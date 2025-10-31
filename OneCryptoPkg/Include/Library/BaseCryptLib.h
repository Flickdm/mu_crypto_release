/** @file
  Base Crypto Library Interface Definition

  This header defines the standard crypto library interface that can be
  implemented by different crypto providers (OpenSSL, SymCrypt, etc.).

  Copyright (c) Microsoft Corporation. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BASE_CRYPT_LIB_H__
#define __BASE_CRYPT_LIB_H__

#include <Base.h>

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
  );

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
  );

/**
  Check if the crypto library is initialized.
  
  @retval TRUE   Crypto library is initialized.
  @retval FALSE  Crypto library is not initialized.
**/
BOOLEAN
EFIAPI
BaseCryptLibIsInitialized (
  VOID
  );

//
// Hash Functions
//

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-256 hash operations.

  @return  The size, in bytes, of the context buffer required for SHA-256 hash operations.
**/
UINTN
EFIAPI
Sha256GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory pointed by Sha256Context as SHA-256 hash context for
  subsequent use.

  If Sha256Context is NULL, then return FALSE.

  @param[out]  Sha256Context  Pointer to SHA-256 context being initialized.

  @retval TRUE   SHA-256 context initialization succeeded.
  @retval FALSE  SHA-256 context initialization failed.
**/
BOOLEAN
EFIAPI
Sha256Init (
  OUT  VOID  *Sha256Context
  );

/**
  Makes a copy of an existing SHA-256 context.

  If Sha256Context is NULL, then return FALSE.
  If NewSha256Context is NULL, then return FALSE.

  @param[in]  Sha256Context     Pointer to SHA-256 context being copied.
  @param[out] NewSha256Context  Pointer to new SHA-256 context.

  @retval TRUE   SHA-256 context copy succeeded.
  @retval FALSE  SHA-256 context copy failed.
**/
BOOLEAN
EFIAPI
Sha256Duplicate (
  IN   CONST VOID  *Sha256Context,
  OUT  VOID        *NewSha256Context
  );

/**
  Digests the input data and updates SHA-256 context.

  This function performs SHA-256 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  SHA-256 context should be already correctly initialized by Sha256Init(), and should not be finalized
  by Sha256Final(). Behavior with invalid context is undefined.

  If Sha256Context is NULL, then return FALSE.

  @param[in, out]  Sha256Context  Pointer to the SHA-256 context.
  @param[in]       Data           Pointer to the buffer containing the data to be hashed.
  @param[in]       DataSize       Size of Data buffer in bytes.

  @retval TRUE   SHA-256 data digest succeeded.
  @retval FALSE  SHA-256 data digest failed.
**/
BOOLEAN
EFIAPI
Sha256Update (
  IN OUT  VOID        *Sha256Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

/**
  Completes computation of the SHA-256 digest value.

  This function completes SHA-256 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the SHA-256 context cannot
  be used again.
  SHA-256 context should be already correctly initialized by Sha256Init(), and should not be
  finalized by Sha256Final(). Behavior with invalid context is undefined.

  If Sha256Context is NULL, then return FALSE.
  If HashValue is NULL, then return FALSE.

  @param[in, out]  Sha256Context  Pointer to the SHA-256 context.
  @param[out]      HashValue      Pointer to a buffer that receives the SHA-256 digest
                                  value (32 bytes).

  @retval TRUE   SHA-256 digest computation succeeded.
  @retval FALSE  SHA-256 digest computation failed.
**/
BOOLEAN
EFIAPI
Sha256Final (
  IN OUT  VOID   *Sha256Context,
  OUT     UINT8  *HashValue
  );

//
// HMAC Functions
//

/**
  Computes the HMAC-SHA256 digest of a input data buffer.

  This function performs the HMAC-SHA256 digest of a given data buffer, and places
  the digest value into the specified memory.

  If this interface is not supported, then return FALSE.

  @param[in]   Data        Pointer to the buffer containing the data to be digested.
  @param[in]   DataSize    Size of Data buffer in bytes.
  @param[in]   Key         Pointer to the user-supplied key.
  @param[in]   KeySize     Key size in bytes.
  @param[out]  HmacValue   Pointer to a buffer that receives the HMAC-SHA256 digest
                           value (32 bytes).

  @retval TRUE   HMAC-SHA256 digest computation succeeded.
  @retval FALSE  HMAC-SHA256 digest computation failed.
  @retval FALSE  This interface is not supported.
**/
BOOLEAN
EFIAPI
HmacSha256All (
  IN   CONST VOID   *Data,
  IN   UINTN        DataSize,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeySize,
  OUT  UINT8        *HmacValue
  );

//
// Additional crypto functions can be added here as needed
// This is just a basic interface to start with
//

#endif // __BASE_CRYPT_LIB_H__
