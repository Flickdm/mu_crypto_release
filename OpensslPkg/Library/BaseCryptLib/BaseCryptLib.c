/** @file
  Base Crypto Library Implementation using OpenSSL

  This provides the OpenSSL implementation of BaseCryptLib interface.

  Copyright (c) Microsoft Corporation. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseCryptLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <openssl/opensslv.h>

//
// Global initialization state
//
STATIC BOOLEAN  mBaseCryptLibInitialized = FALSE;

/**
  Initialize the crypto library.
  
  This function must be called before any other crypto operations.
  For providers that don't support constructors (like minimal standalone MM),
  this provides explicit initialization.
  
  @retval TRUE   Crypto library initialized successfully.
  @retval FALSE  Crypto library initialization failed.
**/
BOOLEAN
EFIAPI
BaseCryptLibInitialize (
  VOID
  )
{
  if (mBaseCryptLibInitialized) {
    return TRUE;
  }

  //
  // Initialize OpenSSL components
  // Most initialization is handled by OpensslLib, but we mark as initialized
  //
  
  mBaseCryptLibInitialized = TRUE;
  DEBUG ((DEBUG_INFO, "BaseCryptLib: OpenSSL crypto library initialized\n"));
  
  return TRUE;
}

/**
  Finalize the crypto library.
  
  This function should be called when crypto operations are no longer needed.
  
  @retval TRUE   Crypto library finalized successfully.
  @retval FALSE  Crypto library finalization failed.
**/
BOOLEAN
EFIAPI
BaseCryptLibFinalize (
  VOID
  )
{
  if (!mBaseCryptLibInitialized) {
    return TRUE;
  }

  //
  // Cleanup OpenSSL resources if needed
  //
  
  mBaseCryptLibInitialized = FALSE;
  DEBUG ((DEBUG_INFO, "BaseCryptLib: OpenSSL crypto library finalized\n"));
  
  return TRUE;
}

/**
  Check if the crypto library is initialized.
  
  @retval TRUE   Crypto library is initialized.
  @retval FALSE  Crypto library is not initialized.
**/
BOOLEAN
EFIAPI
BaseCryptLibIsInitialized (
  VOID
  )
{
  return mBaseCryptLibInitialized;
}

/**
  Gets the OpenSSL version information.
  
  @return  Pointer to OpenSSL version string.
**/
CONST CHAR8 *
EFIAPI
GetOpenSslVersionText (
  VOID
  )
{
  return OPENSSL_VERSION_TEXT;
}

/**
  Gets the OpenSSL version number.
  
  @return  OpenSSL version number.
**/
UINTN
EFIAPI
GetOpenSslVersionNumber (
  VOID
  )
{
  return OPENSSL_VERSION_NUMBER;
}
