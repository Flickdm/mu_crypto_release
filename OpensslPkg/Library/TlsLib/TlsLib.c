/** @file
  TLS library implementation using OpenSSL.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <openssl/opensslv.h>

//
// Global flag to track initialization state
//
STATIC BOOLEAN  mTlsLibInitialized = FALSE;

/**
  Initialize the TLS library.
  
  For providers that don't support constructors (like minimal standalone MM),
  this provides explicit initialization.
  
  @retval TRUE   TLS library initialized successfully.
  @retval FALSE  TLS library initialization failed.
**/
BOOLEAN
EFIAPI
TlsLibInitialize (
  VOID
  )
{
  if (mTlsLibInitialized) {
    return TRUE;
  }

  //
  // Initialize TLS components
  // Most initialization is handled by OpensslLib and BaseCryptLib
  //
  
  mTlsLibInitialized = TRUE;
  DEBUG ((DEBUG_INFO, "TlsLib: TLS library initialized\n"));
  
  return TRUE;
}

/**
  Finalize the TLS library.
  
  This function should be called when TLS operations are no longer needed.
  
  @retval TRUE   TLS library finalized successfully.
  @retval FALSE  TLS library finalization failed.
**/
BOOLEAN
EFIAPI
TlsLibFinalize (
  VOID
  )
{
  if (!mTlsLibInitialized) {
    return TRUE;
  }

  //
  // Cleanup TLS resources if needed
  //
  
  mTlsLibInitialized = FALSE;
  DEBUG ((DEBUG_INFO, "TlsLib: TLS library finalized\n"));
  
  return TRUE;
}

/**
  Check if the TLS library is initialized.
  
  @retval TRUE   TLS library is initialized.
  @retval FALSE  TLS library is not initialized.
**/
BOOLEAN
EFIAPI
TlsLibIsInitialized (
  VOID
  )
{
  return mTlsLibInitialized;
}