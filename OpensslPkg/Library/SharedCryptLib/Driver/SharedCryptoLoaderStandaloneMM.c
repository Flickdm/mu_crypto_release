/** @file
  SharedCryptoLoaderStandaloneMM.c

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  This file contains the implementation of the SharedCryptoLoader StandaloneMM driver,
  which is responsible for loading and initializing the shared cryptographic
  library and its dependencies.

**/

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/MmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrePiLib.h>
#include <Library/RngLib.h>

#include <Guid/FirmwareFileSystem2.h>
#include <Protocol/FirmwareVolume2.h>
#include <Ppi/FirmwareVolumeInfo.h>

#include <Protocol/SharedCryptoProtocol.h>
#include <Library/SharedCryptoDependencySupport.h>
#include "SharedLoaderShim.h"

#define EFI_SECTION_PE32  0x10

//
// Exposing gDriverDependencies to reduce phase specific code
//
DRIVER_DEPENDENCIES  *gDriverDependencies = NULL;
//
// The dependencies of the shared library, must live as long
// as the shared code is used
//
SHARED_DEPENDENCIES  *mSharedDepends = NULL;
//
// Crypto protocol for the shared library
//
SHARED_CRYPTO_PROTOCOL mSharedCryptoProtocol;

UINT64
EFIAPI
GetVersion (
  VOID
  )
{
  return PACK_VERSION (VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
}

/**
 * @brief Asserts that the given EFI_STATUS is not an error.
 *
 * This macro checks if the provided EFI_STATUS value indicates an error.
 * If the status is an error, it triggers an assertion failure.
 *
 * @param Status The EFI_STATUS value to be checked.
 */
VOID
EFIAPI
AssertEfiError (
  BOOLEAN  Expression
  )
{
  ASSERT_EFI_ERROR (Expression);
}

/**
 * @brief Installs shared dependencies required for the application.
 *
 * This function handles the installation of shared dependencies that are
 * necessary for the application to run properly. It ensures that all
 * required libraries and packages are installed and up to date.
 *
 * @param dependencies A list of dependencies to be installed.
 */
VOID
InstallSharedDependencies (
  OUT SHARED_DEPENDENCIES  *SharedDepends
  )
{
  // TODO add a version number in case the dependencies grow
  SharedDepends->AllocatePool      = AllocatePool;
  SharedDepends->FreePool          = FreePool;
  SharedDepends->ASSERT            = AssertEfiError;
  SharedDepends->DebugPrint        = DebugPrint;
  SharedDepends->GetTime           = NULL;
  SharedDepends->GetRandomNumber64 = GetRandomNumber64;
}

/**
 * @brief Installs the necessary driver dependencies.
 *
 * This function is responsible for installing all the required dependencies
 * for the driver to function correctly. It ensures that all the necessary
 * libraries and components are present and properly configured.
 *
 * @param driverPath The path to the driver that requires dependencies.
 * @param dependencies A list of dependencies that need to be installed.
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
VOID
InstallDriverDependencies (
   VOID
  )
{
  gDriverDependencies->AllocatePages  = gMmst->MmAllocatePages;
  gDriverDependencies->FreePages      = gMmst->MmFreePages;
  gDriverDependencies->LocateProtocol = gMmst->MmLocateProtocol;
  gDriverDependencies->AllocatePool   = gMmst->MmAllocatePool;
  gDriverDependencies->FreePool       = gMmst->MmFreePool;
}

EFI_STATUS
DiscoverCryptoBinary (
  IN EFI_GUID  *TargetGuid,
  OUT VOID     **OutSectionData,
  OUT UINT64   *OutSectionDataSize
  )
{
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_FIRMWARE_VOLUME_HEADER  *FwVolHeader;
  EFI_FFS_FILE_HEADER         *FileHeader;
  EFI_STATUS                  Status;
  VOID                        *SectionData;
  BOOLEAN                     Found;
  UINTN                       FileSize;

  Found = FALSE;

  Hob.Raw = GetHobList ();
  if (Hob.Raw == NULL) {
    return EFI_NOT_FOUND;
  }

  do {
    Hob.Raw = GetNextHob (EFI_HOB_TYPE_FV, Hob.Raw);
    if (Hob.Raw != NULL) {
      FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)(Hob.FirmwareVolume->BaseAddress);

      FileHeader = NULL;
      Status     = FfsFindNextFile (EFI_FV_FILETYPE_APPLICATION, FwVolHeader, &FileHeader);

      while (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "Found EFI Application: %g\n", &FileHeader->Name));

        if (CompareGuid (&FileHeader->Name, TargetGuid)) {
          DEBUG ((DEBUG_INFO, "Found EFI Application with matching GUID.\n"));
          Found  = TRUE;
          Status = EFI_SUCCESS;
          break;
        }

        Status = FfsFindNextFile (EFI_FV_FILETYPE_APPLICATION, FwVolHeader, &FileHeader);
      }

      Hob.Raw = GetNextHob (EFI_HOB_TYPE_FV, GET_NEXT_HOB (Hob));
    }
  } while (Hob.Raw != NULL);

  if (!Found && !EFI_ERROR (Status)) {
    Status = EFI_NOT_FOUND;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to find file by GUID: %r\n", Status));
    return Status;
  }

  Status = FfsFindSectionData (EFI_SECTION_PE32, FileHeader, &SectionData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to find section with known GUID: %r\n", Status));
    return Status;
  }

  FileSize = 0;
  CopyMem (&FileSize, FileHeader->Size, sizeof (FileHeader->Size));

  *OutSectionDataSize = FileSize - sizeof (EFI_FFS_FILE_HEADER);;
  *OutSectionData = SectionData;

  return EFI_SUCCESS;
}

/**
 * Entry point for the DXE (Driver Execution Environment) phase.
 *
 * This function is the main entry point for the DXE phase of the UEFI (Unified Extensible Firmware Interface) firmware.
 * It is responsible for initializing the DXE environment and executing the DXE drivers.
 *
 * @param ImageHandle  The firmware allocated handle for the EFI image.
 * @param SystemTable  A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS           The entry point is executed successfully.
 * @retval EFI_LOAD_ERROR        Failed to load the DXE environment.
 * @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
 */
EFI_STATUS
EFIAPI
MmEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_MM_SYSTEM_TABLE  *MmSystemTable
  )
{
  EFI_STATUS   Status;
  VOID         *SectionData;
  UINTN        SectionSize;
  CONSTRUCTOR  Constructor;

  //
  // This must match the INF for SharedCryptoBin
  //
  EFI_GUID  SharedLibGuid = {
    0x76ABA88D, 0x9D16, 0x49A2, { 0xAA, 0x3A, 0xDB, 0x61, 0x12, 0xFA, 0xC5, 0xCB }
  };

  //
  // Initialize the Driver dependencies
  //
  if (gDriverDependencies == NULL) {
    gDriverDependencies = AllocatePool (sizeof (*gDriverDependencies));
    if (gDriverDependencies == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    InstallDriverDependencies ();
  }

  //
  // Initialize the Shared dependencies
  //

  if (mSharedDepends == NULL) {
    mSharedDepends = AllocatePool (sizeof (*mSharedDepends));
    if (mSharedDepends == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    InstallSharedDependencies (mSharedDepends);
  }

  //
  // Print out the GUID of the shared library
  //
  DEBUG ((DEBUG_INFO, "Searching for Shared library GUID: %g\n", SharedLibGuid));

  Status = DiscoverCryptoBinary (&SharedLibGuid, &SectionData, &SectionSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to discover crypto binary: %r\n", Status));
    goto Exit;
  }

  //
  // Load the binary and get the entry point
  // TODO: This should be able to be replaced if we rewrite the Uefi Loader
  // and create a new entry point
  //
  Status = LoaderEntryPoint (SectionData, SectionSize, &Constructor);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to load shared library: %r\n", Status));
    goto Exit;
  }

  //
  // Provide the requested version to the constructor
  //
  mSharedCryptoProtocol.GetVersion = GetVersion;

  //
  // Call library constructor to generate the protocol
  //
  Status = Constructor (mSharedDepends, &mSharedCryptoProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to call LibConstructor: %r\n", Status));
    goto Exit;
  }

  /*
  Status = gMmst->MmInstallProtocolInterface (
    &ImageHandle,
    &gSharedCryptoProtocolGuid,
    EFI_NATIVE_INTERFACE,
    (VOID *)&mSharedCryptoProtocol
  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to install protocol: %r\n", Status));
    goto Exit;
  }*/

  DEBUG ((DEBUG_INFO, "SharedCrypto Protocol installed successfully.\n"));

  Status = EFI_SUCCESS;

Exit:

  //
  // In the standalone MM environment, section data is pointing at a FFS file
  // therefore it is not allocated by the MM services, so we do not free it.
  //

  //
  // The driver dependencies may be freed regardless of the status
  //
  if (gDriverDependencies != NULL) {
    FreePool (gDriverDependencies);
  }

  //
  // The dependendencies that the shared library needs may not be freed unless
  // there was an error. If there is no Error then the memory must live long past this driver.
  //
  if ((Status != EFI_SUCCESS) && (gSharedDepends != NULL)) {
    FreePool (gSharedDepends);
  }

  return Status;
}
