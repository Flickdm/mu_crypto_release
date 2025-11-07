/** @file  
  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef ONE_CRYPTO_LOADER_DRIVER_SUPPORT_H
#define ONE_CRYPTO_LOADER_DRIVER_SUPPORT_H

#include <Uefi.h>
#include "Library/OneCryptoDependencySupport.h"
#include <Protocol/LoadedImage.h>

//
// These represent the dependencies that the driver needs to function
// correctly regardless of the phase it is loaded in.
//
typedef struct _DRIVER_DEPENDENCIES {
  EFI_LOCATE_PROTOCOL    LocateProtocol;
  EFI_ALLOCATE_PAGES     AllocatePages;
  EFI_FREE_PAGES         FreePages;
  EFI_ALLOCATE_POOL      AllocatePool;
  EFI_FREE_POOL          FreePool;
} DRIVER_DEPENDENCIES;

//
// Global variable to hold the driver dependencies
//
extern DRIVER_DEPENDENCIES  *gDriverDependencies;

#endif // ONE_CRYPTO_LOADER_DRIVER_SUPPORT_H
