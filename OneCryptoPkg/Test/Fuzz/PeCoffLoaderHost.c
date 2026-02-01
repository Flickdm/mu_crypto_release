/** @file
  PeCoffLoaderHost.c

  Simple PE/COFF loader for loading OneCrypto binaries on the host system.
  This allows fuzzing the production binary without recompilation.

  This loader:
  1. Reads the PE/COFF file
  2. Allocates memory with proper permissions (RWX for code sections)
  3. Copies sections to their virtual addresses
  4. Resolves the export table to find CryptoEntry

  Note: This is a minimal loader for fuzzing purposes. It does not handle:
  - Relocations (assumes image can be loaded at preferred base)
  - Imports (OneCrypto binary has no imports - it uses dependency injection)
  - TLS, exception handling, etc.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PeCoffLoaderHost.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

/**
  Load a PE/COFF binary from a file.

  @param[in]  FilePath    Path to the .efi file to load.
  @param[out] ImageBase   Receives the base address of the loaded image.
  @param[out] ImageSize   Receives the size of the loaded image.

  @retval 0   Success
  @retval -1  Failed to load
**/
int
PeCoffLoadImage (
  const char  *FilePath,
  void        **ImageBase,
  size_t      *ImageSize
  )
{
  FILE                   *File;
  uint8_t                *FileData;
  size_t                 FileSize;
  uint16_t               DosSignature;
  uint32_t               PeOffset;
  uint32_t               PeSignature;
  PE_COFF_FILE_HEADER    *FileHeader;
  PE_OPTIONAL_HEADER_64  *OptHeader;
  PE_SECTION_HEADER      *Sections;
  uint8_t                *LoadedImage;
  uint32_t               i;
  uint32_t               SizeOfImage;
  uint16_t               NumberOfSections;
  uint16_t               SizeOfOptionalHeader;
  uint32_t               SizeOfHeaders;

  if ((FilePath == NULL) || (ImageBase == NULL) || (ImageSize == NULL)) {
    return -1;
  }

  *ImageBase = NULL;
  *ImageSize = 0;

  //
  // Read the entire file
  //
  File = fopen (FilePath, "rb");
  if (File == NULL) {
    fprintf (stderr, "PeCoffLoader: Cannot open file: %s\n", FilePath);
    return -1;
  }

  fseek (File, 0, SEEK_END);
  FileSize = ftell (File);
  fseek (File, 0, SEEK_SET);

  FileData = (uint8_t *)malloc (FileSize);
  if (FileData == NULL) {
    fprintf (stderr, "PeCoffLoader: Cannot allocate %zu bytes for file\n", FileSize);
    fclose (File);
    return -1;
  }

  if (fread (FileData, 1, FileSize, File) != FileSize) {
    fprintf (stderr, "PeCoffLoader: Cannot read file\n");
    free (FileData);
    fclose (File);
    return -1;
  }

  fclose (File);

  //
  // Verify DOS signature (MZ)
  //
  if (FileSize < 64) {
    fprintf (stderr, "PeCoffLoader: File too small\n");
    free (FileData);
    return -1;
  }

  DosSignature = *(uint16_t *)FileData;
  if (DosSignature != 0x5A4D) {  // "MZ"
    fprintf (stderr, "PeCoffLoader: Invalid DOS signature: 0x%04x\n", DosSignature);
    free (FileData);
    return -1;
  }

  //
  // Get PE offset from DOS header
  //
  PeOffset = *(uint32_t *)(FileData + 0x3C);
  if (PeOffset + sizeof (uint32_t) + sizeof (PE_COFF_FILE_HEADER) > FileSize) {
    fprintf (stderr, "PeCoffLoader: PE offset out of bounds\n");
    free (FileData);
    return -1;
  }

  //
  // Verify PE signature
  //
  PeSignature = *(uint32_t *)(FileData + PeOffset);
  if (PeSignature != PE_SIGNATURE) {
    fprintf (stderr, "PeCoffLoader: Invalid PE signature: 0x%08x\n", PeSignature);
    free (FileData);
    return -1;
  }

  //
  // Parse PE headers
  //
  FileHeader = (PE_COFF_FILE_HEADER *)(FileData + PeOffset + 4);
  OptHeader  = (PE_OPTIONAL_HEADER_64 *)((uint8_t *)FileHeader + sizeof (PE_COFF_FILE_HEADER));

  if (OptHeader->Magic != PE_OPTIONAL_HDR64_MAGIC) {
    fprintf (stderr, "PeCoffLoader: Only PE32+ (64-bit) images supported, got magic 0x%04x\n", OptHeader->Magic);
    free (FileData);
    return -1;
  }

  //
  // Save values we need
  //
  SizeOfImage = OptHeader->SizeOfImage;
  SizeOfHeaders = OptHeader->SizeOfHeaders;
  NumberOfSections = FileHeader->NumberOfSections;
  SizeOfOptionalHeader = FileHeader->SizeOfOptionalHeader;

  fprintf (stderr, "PeCoffLoader: Image size: %u bytes\n", SizeOfImage);
  fprintf (stderr, "PeCoffLoader: Number of sections: %u\n", NumberOfSections);
  fprintf (stderr, "PeCoffLoader: Preferred ImageBase: 0x%llx\n", (unsigned long long)OptHeader->ImageBase);

  //
  // Allocate memory for the loaded image with RWX permissions
  // (needed because we'll be executing code from it)
  //
  LoadedImage = (uint8_t *)mmap (
                             NULL,
                             SizeOfImage,
                             PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             -1,
                             0
                             );
  if (LoadedImage == MAP_FAILED) {
    fprintf (stderr, "PeCoffLoader: Cannot allocate %u bytes for image\n", SizeOfImage);
    free (FileData);
    return -1;
  }

  //
  // Zero the image
  //
  memset (LoadedImage, 0, SizeOfImage);

  //
  // Copy headers
  //
  memcpy (LoadedImage, FileData, SizeOfHeaders);

  //
  // Copy sections
  //
  Sections = (PE_SECTION_HEADER *)((uint8_t *)OptHeader + SizeOfOptionalHeader);

  for (i = 0; i < NumberOfSections; i++) {
    PE_SECTION_HEADER  *Section = &Sections[i];

    fprintf (
      stderr,
      "PeCoffLoader: Section %.8s: VA=0x%x Size=0x%x RawSize=0x%x\n",
      Section->Name,
      Section->VirtualAddress,
      Section->VirtualSize,
      Section->SizeOfRawData
      );

    if (Section->SizeOfRawData > 0) {
      if (Section->PointerToRawData + Section->SizeOfRawData > FileSize) {
        fprintf (stderr, "PeCoffLoader: Section data out of bounds\n");
        munmap (LoadedImage, SizeOfImage);
        free (FileData);
        return -1;
      }

      if (Section->VirtualAddress + Section->SizeOfRawData > SizeOfImage) {
        fprintf (stderr, "PeCoffLoader: Section VA out of bounds\n");
        munmap (LoadedImage, SizeOfImage);
        free (FileData);
        return -1;
      }

      memcpy (
        LoadedImage + Section->VirtualAddress,
        FileData + Section->PointerToRawData,
        Section->SizeOfRawData
        );
    }
  }

  //
  // Apply relocations if the image is loaded at a different address
  //
  {
    int64_t         Delta;
    PE_DATA_DIRECTORY  *DataDirs;
    PE_DATA_DIRECTORY  *RelocDir;
    PE_BASE_RELOCATION_BLOCK  *RelocBlock;
    uint32_t        RelocSize;
    uint8_t         *RelocEnd;

    Delta = (int64_t)((uintptr_t)LoadedImage - OptHeader->ImageBase);

    if (Delta != 0) {
      fprintf (stderr, "PeCoffLoader: Applying relocations (delta=0x%llx)\n", (unsigned long long)Delta);

      // Get relocation directory - it's at index 5 in the data directories
      DataDirs = (PE_DATA_DIRECTORY *)((uint8_t *)&OptHeader->NumberOfRvaAndSizes + sizeof (uint32_t));
      
      if (OptHeader->NumberOfRvaAndSizes > 5) {
        RelocDir = &DataDirs[5];  // PE_DIRECTORY_ENTRY_BASERELOC = 5
        
        if (RelocDir->VirtualAddress != 0 && RelocDir->Size > 0) {
          RelocBlock = (PE_BASE_RELOCATION_BLOCK *)(LoadedImage + RelocDir->VirtualAddress);
          RelocSize = RelocDir->Size;
          RelocEnd = (uint8_t *)RelocBlock + RelocSize;

          while ((uint8_t *)RelocBlock < RelocEnd && RelocBlock->SizeOfBlock > 0) {
            uint16_t  *TypeOffset;
            uint32_t  NumRelocs;
            uint32_t  j;

            NumRelocs = (RelocBlock->SizeOfBlock - sizeof (PE_BASE_RELOCATION_BLOCK)) / sizeof (uint16_t);
            TypeOffset = (uint16_t *)((uint8_t *)RelocBlock + sizeof (PE_BASE_RELOCATION_BLOCK));

            for (j = 0; j < NumRelocs; j++) {
              uint16_t  Type   = TypeOffset[j] >> 12;
              uint16_t  Offset = TypeOffset[j] & 0xFFF;
              uint8_t   *FixupAddr = LoadedImage + RelocBlock->VirtualAddress + Offset;

              switch (Type) {
                case PE_REL_BASED_ABSOLUTE:
                  // Padding, skip
                  break;
                case PE_REL_BASED_DIR64:
                  *(uint64_t *)FixupAddr += Delta;
                  break;
                case PE_REL_BASED_HIGHLOW:
                  *(uint32_t *)FixupAddr += (uint32_t)Delta;
                  break;
                default:
                  fprintf (stderr, "PeCoffLoader: Unsupported relocation type %u\n", Type);
                  break;
              }
            }

            RelocBlock = (PE_BASE_RELOCATION_BLOCK *)((uint8_t *)RelocBlock + RelocBlock->SizeOfBlock);
          }

          fprintf (stderr, "PeCoffLoader: Relocations applied\n");
        }
      }
    }
  }

  free (FileData);

  *ImageBase = LoadedImage;
  *ImageSize = SizeOfImage;

  fprintf (stderr, "PeCoffLoader: Image loaded at %p\n", LoadedImage);

  return 0;
}

/**
  Find an exported function by name in a loaded PE/COFF image.

  @param[in]  ImageBase   Base address of the loaded image.
  @param[in]  Name        Name of the export to find.

  @return Pointer to the function, or NULL if not found.
**/
void *
PeCoffFindExport (
  void        *ImageBase,
  const char  *Name
  )
{
  uint8_t                *Base;
  uint32_t               PeOffset;
  PE_COFF_FILE_HEADER    *FileHeader;
  PE_OPTIONAL_HEADER_64  *OptHeader;
  PE_DATA_DIRECTORY      *ExportDir;
  PE_EXPORT_DIRECTORY    *ExportTable;
  uint32_t               *Functions;
  uint32_t               *Names;
  uint16_t               *Ordinals;
  uint32_t               i;

  if ((ImageBase == NULL) || (Name == NULL)) {
    return NULL;
  }

  Base = (uint8_t *)ImageBase;

  //
  // Navigate to optional header
  //
  PeOffset   = *(uint32_t *)(Base + 0x3C);
  FileHeader = (PE_COFF_FILE_HEADER *)(Base + PeOffset + 4);
  OptHeader  = (PE_OPTIONAL_HEADER_64 *)((uint8_t *)FileHeader + sizeof (PE_COFF_FILE_HEADER));

  //
  // Get export directory
  //
  if (OptHeader->NumberOfRvaAndSizes <= PE_DIRECTORY_ENTRY_EXPORT) {
    fprintf (stderr, "PeCoffLoader: No export directory\n");
    return NULL;
  }

  ExportDir = (PE_DATA_DIRECTORY *)((uint8_t *)OptHeader + 
              offsetof (PE_OPTIONAL_HEADER_64, SizeOfStackReserve) + 
              sizeof (uint64_t) * 4 + sizeof (uint32_t) * 2);  // Skip to data directories

  // Actually, the data directories follow NumberOfRvaAndSizes
  ExportDir = (PE_DATA_DIRECTORY *)((uint8_t *)&OptHeader->NumberOfRvaAndSizes + sizeof (uint32_t));

  if (ExportDir[PE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0) {
    fprintf (stderr, "PeCoffLoader: Export directory is empty\n");
    return NULL;
  }

  ExportTable = (PE_EXPORT_DIRECTORY *)(Base + ExportDir[PE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

  fprintf (
    stderr,
    "PeCoffLoader: Export table: %u functions, %u names\n",
    ExportTable->NumberOfFunctions,
    ExportTable->NumberOfNames
    );

  Functions = (uint32_t *)(Base + ExportTable->AddressOfFunctions);
  Names     = (uint32_t *)(Base + ExportTable->AddressOfNames);
  Ordinals  = (uint16_t *)(Base + ExportTable->AddressOfNameOrdinals);

  //
  // Search for the export by name
  //
  for (i = 0; i < ExportTable->NumberOfNames; i++) {
    const char  *ExportName = (const char *)(Base + Names[i]);

    if (strcmp (ExportName, Name) == 0) {
      uint16_t  Ordinal = Ordinals[i];
      uint32_t  FuncRva = Functions[Ordinal];

      fprintf (stderr, "PeCoffLoader: Found export '%s' at RVA 0x%x\n", Name, FuncRva);
      return (void *)(Base + FuncRva);
    }
  }

  fprintf (stderr, "PeCoffLoader: Export '%s' not found\n", Name);
  return NULL;
}

/**
  Unload a PE/COFF image.

  @param[in]  ImageBase   Base address of the loaded image.
**/
void
PeCoffUnloadImage (
  void  *ImageBase
  )
{
  //
  // We need to know the size to unmap, but we don't store it.
  // For now, just leak the memory. In a real implementation,
  // we'd track the allocation size.
  //
  // TODO: Track image size for proper cleanup
  //
  (void)ImageBase;
}
