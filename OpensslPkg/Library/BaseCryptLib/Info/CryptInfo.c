/** @file
  Cryptographic Library Information Implementation.

  This module provides version information for the underlying OpenSSL library.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <Library/BaseCryptLib.h>

/**
  Gets the OpenSSL version information.
  
  This function returns the OpenSSL version string that was used to compile
  the cryptographic library.
  
  @return  Pointer to OpenSSL version string.
**/
CONST CHAR8 *
EFIAPI
GetOpenSslVersionText (
  VOID
  )
{
  // Return the compile-time version string
  return OPENSSL_VERSION_TEXT;
}

/**
  Gets the OpenSSL version number.
  
  This function returns the OpenSSL version number that was used to compile
  the cryptographic library.
  
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
