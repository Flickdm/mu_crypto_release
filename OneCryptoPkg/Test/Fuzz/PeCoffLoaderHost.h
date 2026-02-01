/** @file
  PeCoffLoaderHost.h

  Simple PE/COFF loader for loading OneCrypto binaries on the host system.
  This allows fuzzing the production binary without recompilation.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PE_COFF_LOADER_HOST_H_
#define PE_COFF_LOADER_HOST_H_

#include "HostUefiCompat.h"
#include <stdint.h>
#include <stddef.h>

//
// PE/COFF Header structures (subset needed for loading)
//

#define PE_SIGNATURE  0x00004550  // "PE\0\0"

#pragma pack(push, 1)

typedef struct {
  uint16_t  Machine;
  uint16_t  NumberOfSections;
  uint32_t  TimeDateStamp;
  uint32_t  PointerToSymbolTable;
  uint32_t  NumberOfSymbols;
  uint16_t  SizeOfOptionalHeader;
  uint16_t  Characteristics;
} PE_COFF_FILE_HEADER;

typedef struct {
  uint16_t  Magic;
  uint8_t   MajorLinkerVersion;
  uint8_t   MinorLinkerVersion;
  uint32_t  SizeOfCode;
  uint32_t  SizeOfInitializedData;
  uint32_t  SizeOfUninitializedData;
  uint32_t  AddressOfEntryPoint;
  uint32_t  BaseOfCode;
  uint64_t  ImageBase;
  uint32_t  SectionAlignment;
  uint32_t  FileAlignment;
  uint16_t  MajorOperatingSystemVersion;
  uint16_t  MinorOperatingSystemVersion;
  uint16_t  MajorImageVersion;
  uint16_t  MinorImageVersion;
  uint16_t  MajorSubsystemVersion;
  uint16_t  MinorSubsystemVersion;
  uint32_t  Win32VersionValue;
  uint32_t  SizeOfImage;
  uint32_t  SizeOfHeaders;
  uint32_t  CheckSum;
  uint16_t  Subsystem;
  uint16_t  DllCharacteristics;
  uint64_t  SizeOfStackReserve;
  uint64_t  SizeOfStackCommit;
  uint64_t  SizeOfHeapReserve;
  uint64_t  SizeOfHeapCommit;
  uint32_t  LoaderFlags;
  uint32_t  NumberOfRvaAndSizes;
  // Data directories follow immediately
} PE_OPTIONAL_HEADER_64;

typedef struct {
  uint32_t  VirtualAddress;
  uint32_t  Size;
} PE_DATA_DIRECTORY;

//
// Base relocation block structure
//
typedef struct {
  uint32_t  VirtualAddress;  // Page RVA
  uint32_t  SizeOfBlock;     // Size of block including header
  // uint16_t TypeOffset[...] follows
} PE_BASE_RELOCATION_BLOCK;

// Relocation types
#define PE_REL_BASED_ABSOLUTE  0   // Skip this entry
#define PE_REL_BASED_HIGH      1   // High 16 bits of 32-bit field
#define PE_REL_BASED_LOW       2   // Low 16 bits of 32-bit field
#define PE_REL_BASED_HIGHLOW   3   // All 32 bits of 32-bit field
#define PE_REL_BASED_DIR64     10  // 64-bit field

typedef struct {
  char      Name[8];
  uint32_t  VirtualSize;
  uint32_t  VirtualAddress;
  uint32_t  SizeOfRawData;
  uint32_t  PointerToRawData;
  uint32_t  PointerToRelocations;
  uint32_t  PointerToLinenumbers;
  uint16_t  NumberOfRelocations;
  uint16_t  NumberOfLinenumbers;
  uint32_t  Characteristics;
} PE_SECTION_HEADER;

typedef struct {
  uint32_t  Characteristics;
  uint32_t  TimeDateStamp;
  uint16_t  MajorVersion;
  uint16_t  MinorVersion;
  uint32_t  Name;
  uint32_t  Base;
  uint32_t  NumberOfFunctions;
  uint32_t  NumberOfNames;
  uint32_t  AddressOfFunctions;
  uint32_t  AddressOfNames;
  uint32_t  AddressOfNameOrdinals;
} PE_EXPORT_DIRECTORY;

#pragma pack(pop)

// Magic numbers
#define PE_OPTIONAL_HDR64_MAGIC  0x20b
#define PE_OPTIONAL_HDR32_MAGIC  0x10b

// Data directory indices
#define PE_DIRECTORY_ENTRY_EXPORT  0

// Section characteristics
#define PE_SCN_MEM_EXECUTE  0x20000000
#define PE_SCN_MEM_READ     0x40000000
#define PE_SCN_MEM_WRITE    0x80000000

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
  );

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
  );

/**
  Unload a PE/COFF image.

  @param[in]  ImageBase   Base address of the loaded image.
**/
void
PeCoffUnloadImage (
  void  *ImageBase
  );

#endif // PE_COFF_LOADER_HOST_H_
