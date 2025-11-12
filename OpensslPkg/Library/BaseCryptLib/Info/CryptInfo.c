/** @file
  Cryptographic Library Information Implementation.

  This module provides version information for the underlying OpenSSL library.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include "InternalCryptLib.h"

/**
  Gets the cryptographic provider version information.
  
  This function returns the version string of the cryptographic provider.
  For this OpenSSL implementation, it returns the OpenSSL version string.
  
  @return  Pointer to cryptographic provider version string.
**/
CONST CHAR8 *
EFIAPI
GetCryptoProviderVersionText (
  VOID
  )
{
  // Return the compile-time version string
  return OPENSSL_VERSION_TEXT;
}

/**
  Gets the cryptographic provider version number.
  
  This function returns the version number of the cryptographic provider.
  For this OpenSSL implementation, it returns the OpenSSL version number.
  
  @return  Cryptographic provider version number.
**/
UINTN
EFIAPI
GetCryptoProviderVersionNumber (
  VOID
  )
{
  return OPENSSL_VERSION_NUMBER;
}
