/** @file
  SharedCryptoDefinitions.h

  This file contains the definitions and constants used in the shared cryptographic library that
  are shared across different headers.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef SHARED_CRYPT_LIB_DEFS_H_
#define SHARED_CRYPT_LIB_DEFS_H_

#include <Uefi.h>

#define CRYPTO_NID_NULL  0x0000

// Hash
#define CRYPTO_NID_SHA256  0x0001
#define CRYPTO_NID_SHA384  0x0002
#define CRYPTO_NID_SHA512  0x0003

// Key Exchange
#define CRYPTO_NID_SECP256R1  0x0204
#define CRYPTO_NID_SECP384R1  0x0205
#define CRYPTO_NID_SECP521R1  0x0206

///
/// MD5 digest size in bytes
///
#define MD5_DIGEST_SIZE  16

///
/// SHA-1 digest size in bytes.
///
#define SHA1_DIGEST_SIZE  20

///
/// SHA-256 digest size in bytes
///
#define SHA256_DIGEST_SIZE  32

///
/// SHA-384 digest size in bytes
///
#define SHA384_DIGEST_SIZE  48

///
/// SHA-512 digest size in bytes
///
#define SHA512_DIGEST_SIZE  64

///
/// SM3 digest size in bytes
///
#define SM3_256_DIGEST_SIZE  32

///
/// TDES block size in bytes
///
#define TDES_BLOCK_SIZE  8

///
/// AES block size in bytes
///
#define AES_BLOCK_SIZE  16

//
// Pack the version number into a single UINT64
//
#define PACK_VERSION(Major, Minor, Revision) \
  (((UINT64)(Major) << 32) | ((UINT64)(Minor) << 16) | (UINT64)(Revision))

//
// Unpack the version number from a single UINT64
//
#define UNPACK_VERSION(Version, Major, Minor, Revision) \
  do { \
    Major = (UINT32)((Version) >> 32); \
    Minor = (UINT32)(((Version) >> 16) & 0xFFFF); \
    Revision = (UINT32)((Version) & 0xFFFF); \
  } while (0)

///
/// RSA Key Tags Definition used in RsaSetKey() function for key component identification.
///
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

#endif // SHARED_CRYPT_LIB_DEFS_H_
