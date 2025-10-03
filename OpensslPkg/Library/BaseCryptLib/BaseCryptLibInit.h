/** @file
  BaseCryptLib explicit initialization functions.

  This header provides explicit initialization/finalization functions
  for BaseCryptLib since library constructors/destructors are not
  available with MinimalStandaloneMmDriverEntryPoint.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BASE_CRYPT_LIB_INIT_H__
#define __BASE_CRYPT_LIB_INIT_H__

#include <Uefi/UefiBaseType.h>

/**
  Initializes the BaseCryptLib library.
  
  This function must be called explicitly since library constructors
  are not available with MinimalStandaloneMmDriverEntryPoint.
  
  @retval EFI_SUCCESS           Library initialized successfully
  @retval EFI_ALREADY_STARTED   Library was already initialized
  @retval EFI_DEVICE_ERROR      Initialization failed
**/
EFI_STATUS
EFIAPI
BaseCryptLibInitialize (
  VOID
  );

/**
  Cleanup the BaseCryptLib library.
  
  This function should be called explicitly before shutdown since
  library destructors are not available with MinimalStandaloneMmDriverEntryPoint.
  
  @retval EFI_SUCCESS       Library cleaned up successfully
  @retval EFI_NOT_STARTED   Library was not initialized
**/
EFI_STATUS
EFIAPI
BaseCryptLibFinalize (
  VOID
  );

/**
  Check if BaseCryptLib is initialized.
  
  @retval TRUE   Library is initialized
  @retval FALSE  Library is not initialized
**/
BOOLEAN
EFIAPI
BaseCryptLibIsInitialized (
  VOID
  );

#endif // __BASE_CRYPT_LIB_INIT_H__