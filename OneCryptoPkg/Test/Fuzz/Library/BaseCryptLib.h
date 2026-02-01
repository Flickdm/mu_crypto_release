/** @file
  BaseCryptLib.h - Minimal stub for host fuzzing

  This stub provides the minimum definitions needed for OneCrypto.h to compile
  on a Linux host for fuzzing purposes.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BASE_CRYPT_LIB_H__
#define __BASE_CRYPT_LIB_H__

// All basic types are already defined by HostUefiCompat.h which must be included first

//
// RSA Key Tags Definition
//
typedef enum {
  RsaKeyN,      ///< RSA public Modulus (N)
  RsaKeyE,      ///< RSA Public exponent (e)
  RsaKeyD,      ///< RSA Private exponent (d)
  RsaKeyP,      ///< RSA secret prime factor of Modulus (p)
  RsaKeyQ,      ///< RSA secret prime factor of Modules (q)
  RsaKeyDp,     ///< p's CRT exponent (== d mod (p - 1))
  RsaKeyDq,     ///< q's CRT exponent (== d mod (q - 1))
  RsaKeyQInv    ///< The CRT coefficient (== 1/q mod p)
} RSA_KEY_TAG;

//
// VA_LIST for variadic functions
//
#include <stdarg.h>
typedef va_list VA_LIST;

//
// Macro to get VA_LIST element
//
#define VA_ARG(Marker, TYPE)  va_arg(Marker, TYPE)
#define VA_START(Marker, Parameter)  va_start(Marker, Parameter)
#define VA_END(Marker)  va_end(Marker)

#endif // __BASE_CRYPT_LIB_H__
