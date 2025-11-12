/** @file
  BaseCryptCrtLib.c
  
  Implementation of BaseCryptCrtLib that manages CRT dependencies for
  cryptographic operations.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseCryptCrtLib.h>
#include <Private/OneCryptoDependencySupport.h>

//
// Static pointer to hold the dependencies
//
STATIC ONE_CRYPTO_DEPENDENCIES  *mCryptoDependencies = NULL;

/**
  Initialize the OneCrypto CRT library with the provided dependencies.
  
  This function stores a pointer to the dependency structure which provides
  implementations for memory allocation, time services, random number generation,
  and debugging functions.
  
  @param[in]  Dependencies  Pointer to ONE_CRYPTO_DEPENDENCIES structure containing
                            function pointers for required services.
  
  @retval EFI_SUCCESS           Dependencies were set successfully.
  @retval EFI_INVALID_PARAMETER Dependencies is NULL.
**/
EFI_STATUS
EFIAPI
BaseCryptCrtSetup (
  IN ONE_CRYPTO_DEPENDENCIES  *Dependencies
  )
{
  if (Dependencies == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  mCryptoDependencies = Dependencies;
  return EFI_SUCCESS;
}

/**
  Get the current CRT dependencies.
  
  Internal function to retrieve the dependencies pointer for use by
  other CRT functions.
  
  @return Pointer to ONE_CRYPTO_DEPENDENCIES structure, or NULL if not initialized.
**/
ONE_CRYPTO_DEPENDENCIES *
EFIAPI
GetCryptoDependencies (
  VOID
  )
{
  return mCryptoDependencies;
}
