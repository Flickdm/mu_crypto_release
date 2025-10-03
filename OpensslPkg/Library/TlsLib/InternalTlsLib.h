/** @file
  Internal include file for TlsLib.

Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __INTERNAL_TLS_LIB_H__
#define __INTERNAL_TLS_LIB_H__

#undef _WIN32
#undef _WIN64
#include <CrtLibSupport.h>
#include <Protocol/Tls.h>
#include <IndustryStandard/Tls1.h>
#include <Library/PcdLib.h>
#include <openssl/obj_mac.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <Library/BaseCryptLib.h>

//
// Include the specific crypto headers for functions used by TLS
//
BOOLEAN
EFIAPI
RandomSeed (
  IN  CONST UINT8  *Seed  OPTIONAL,
  IN  UINTN        SeedSize
  );

BOOLEAN
EFIAPI
RandomBytes (
  OUT  UINT8  *Output,
  IN   UINTN  Size
  );

typedef struct {
  //
  // Main SSL Connection which is created by a server or a client
  // per established connection.
  //
  SSL    *Ssl;
  //
  // Memory BIO for the TLS/SSL Reading operations.
  //
  BIO    *InBio;
  //
  // Memory BIO for the TLS/SSL Writing operations.
  //
  BIO    *OutBio;
} TLS_CONNECTION;

//
// Simple safe math functions to avoid external dependencies
//
STATIC
RETURN_STATUS
SafeUintnAdd (
  IN  UINTN  Augend,
  IN  UINTN  Addend,
  OUT UINTN  *Result
  )
{
  if (((UINTN)(~0)) - Augend < Addend) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  *Result = Augend + Addend;
  return RETURN_SUCCESS;
}

STATIC
RETURN_STATUS
SafeUintnMult (
  IN  UINTN  Multiplicand,
  IN  UINTN  Multiplier,
  OUT UINTN  *Result
  )
{
  if (Multiplicand > 0 && (((UINTN)(~0)) / Multiplicand) < Multiplier) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  *Result = Multiplicand * Multiplier;
  return RETURN_SUCCESS;
}

#endif