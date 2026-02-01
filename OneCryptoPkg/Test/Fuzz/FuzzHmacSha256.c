6/** @file
  FuzzHmacSha256.c

  LibFuzzer harness for fuzzing HMAC-SHA256 operations in OneCrypto.

  This harness tests the HMAC-SHA256 implementation by:
  1. Computing HMAC in one call using HmacSha256All
  2. Computing HMAC incrementally using New/SetKey/Update/Final
  3. Testing context duplication

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OneCryptoFuzzHarness.h"
#include <string.h>
#include <stdlib.h>

#define SHA256_DIGEST_SIZE  32
#define MAX_KEY_SIZE        256
#define MAX_UPDATE_SIZE     4096

/**
  LibFuzzer initialization function.

  @param[in]  argc  Argument count (unused).
  @param[in]  argv  Argument vector (unused).

  @retval 0 on success, non-zero on failure.
**/
int
LLVMFuzzerInitialize (
  int   *argc,
  char  ***argv
  )
{
  return OneCryptoFuzzInit ();
}

/**
  LibFuzzer entry point for HMAC-SHA256 fuzzing.

  Input format:
  [1 byte: key length (0-255)] [key bytes] [remaining: data to MAC]

  @param[in]  Data  Pointer to fuzzer-provided input data.
  @param[in]  Size  Size of the input data in bytes.

  @retval 0 Always returns 0 to continue fuzzing.
**/
int
LLVMFuzzerTestOneInput (
  const uint8_t  *Data,
  size_t         Size
  )
{
  ONE_CRYPTO_PROTOCOL  *Crypto;
  UINT8                HmacResult1[SHA256_DIGEST_SIZE];
  UINT8                HmacResult2[SHA256_DIGEST_SIZE];
  UINT8                HmacResult3[SHA256_DIGEST_SIZE];
  VOID                 *Context1;
  VOID                 *Context2;
  const uint8_t        *Key;
  size_t               KeySize;
  const uint8_t        *MsgData;
  size_t               MsgSize;
  const uint8_t        *Ptr;
  size_t               Remaining;
  BOOLEAN              Result;

  //
  // Get the crypto protocol
  //
  Crypto = OneCryptoFuzzGetProtocol ();
  if (Crypto == NULL) {
    return 0;
  }

  //
  // Parse input: first byte is key length, then key, then message
  //
  FUZZ_REQUIRE_MIN_SIZE (Data, Size, 1);

  KeySize = Data[0];
  Data++;
  Size--;

  if (KeySize > Size) {
    KeySize = Size;
  }

  if (KeySize > MAX_KEY_SIZE) {
    KeySize = MAX_KEY_SIZE;
  }

  Key = Data;
  Data += KeySize;
  Size -= KeySize;

  MsgData = Data;
  MsgSize = Size;

  //
  // Test 1: Compute HMAC in one call
  //
  memset (HmacResult1, 0, sizeof (HmacResult1));
  if (KeySize > 0) {
    Result = Crypto->HmacSha256All (MsgData, MsgSize, Key, KeySize, HmacResult1);
    if (!Result) {
      // Operation failed, continue
    }
  }

  //
  // Test 2: Compute HMAC incrementally
  //
  Context1 = Crypto->HmacSha256New ();
  if (Context1 == NULL) {
    return 0;
  }

  memset (HmacResult2, 0, sizeof (HmacResult2));

  if (KeySize > 0) {
    Result = Crypto->HmacSha256SetKey (Context1, Key, KeySize);
    if (!Result) {
      Crypto->HmacSha256Free (Context1);
      return 0;
    }

    //
    // Feed data in chunks
    //
    Ptr       = MsgData;
    Remaining = MsgSize;
    while (Remaining > 0) {
      size_t ChunkSize = (Remaining > MAX_UPDATE_SIZE) ? MAX_UPDATE_SIZE : Remaining;

      //
      // Vary chunk size based on input for more coverage
      //
      if (Remaining > 1) {
        ChunkSize = (Ptr[0] % ChunkSize) + 1;
      }

      Result = Crypto->HmacSha256Update (Context1, Ptr, ChunkSize);
      if (!Result) {
        Crypto->HmacSha256Free (Context1);
        return 0;
      }

      Ptr       += ChunkSize;
      Remaining -= ChunkSize;
    }

    Result = Crypto->HmacSha256Final (Context1, HmacResult2);
    if (Result) {
      //
      // Verify both methods produce the same HMAC
      //
      if (memcmp (HmacResult1, HmacResult2, SHA256_DIGEST_SIZE) != 0) {
        __builtin_trap ();
      }
    }
  }

  //
  // Test 3: Test context duplication
  //
  Result = Crypto->HmacSha256SetKey (Context1, Key, KeySize > 0 ? KeySize : 1);
  if (!Result) {
    Crypto->HmacSha256Free (Context1);
    return 0;
  }

  //
  // Hash half the data
  //
  size_t HalfSize = MsgSize / 2;
  if (HalfSize > 0) {
    Result = Crypto->HmacSha256Update (Context1, MsgData, HalfSize);
    if (!Result) {
      Crypto->HmacSha256Free (Context1);
      return 0;
    }
  }

  //
  // Duplicate the context
  //
  Context2 = Crypto->HmacSha256New ();
  if (Context2 == NULL) {
    Crypto->HmacSha256Free (Context1);
    return 0;
  }

  Result = Crypto->HmacSha256Duplicate (Context1, Context2);
  if (!Result) {
    Crypto->HmacSha256Free (Context1);
    Crypto->HmacSha256Free (Context2);
    return 0;
  }

  //
  // Hash the rest on both contexts
  //
  if (MsgSize - HalfSize > 0) {
    Crypto->HmacSha256Update (Context1, MsgData + HalfSize, MsgSize - HalfSize);
    Crypto->HmacSha256Update (Context2, MsgData + HalfSize, MsgSize - HalfSize);
  }

  //
  // Finalize both and verify they match
  //
  memset (HmacResult2, 0, sizeof (HmacResult2));
  memset (HmacResult3, 0, sizeof (HmacResult3));

  Result = Crypto->HmacSha256Final (Context1, HmacResult2);
  if (Result) {
    Result = Crypto->HmacSha256Final (Context2, HmacResult3);
    if (Result) {
      if (memcmp (HmacResult2, HmacResult3, SHA256_DIGEST_SIZE) != 0) {
        __builtin_trap ();
      }
    }
  }

  Crypto->HmacSha256Free (Context1);
  Crypto->HmacSha256Free (Context2);

  return 0;
}
