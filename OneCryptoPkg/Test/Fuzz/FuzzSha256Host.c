/** @file
  FuzzSha256Host.c

  LibFuzzer harness for fuzzing SHA-256 hash operations in OneCrypto.
  This is the host-compatible version that uses standalone type definitions.

  This harness tests the SHA-256 implementation by:
  1. Hashing arbitrary data in one call using Sha256HashAll
  2. Hashing arbitrary data incrementally using Init/Update/Final
  3. Verifying both methods produce identical results

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OneCryptoFuzzHarnessHost.h"
#include <string.h>
#include <stdlib.h>

#define SHA256_DIGEST_SIZE  32
#define MAX_UPDATE_SIZE     4096

/**
  LibFuzzer initialization function.
  Called once before fuzzing starts.

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
  LibFuzzer entry point.
  Called for each fuzzing iteration with new input data.

  @param[in]  Data  Pointer to fuzzer-provided input data.
  @param[in]  Size  Size of the input data in bytes.

  @retval 0 Always returns 0 (success) to continue fuzzing.
**/
int
LLVMFuzzerTestOneInput (
  const uint8_t  *Data,
  size_t         Size
  )
{
  ONE_CRYPTO_PROTOCOL  *Crypto;
  UINT8                HashResult1[SHA256_DIGEST_SIZE];
  UINT8                HashResult2[SHA256_DIGEST_SIZE];
  UINT8                HashResult3[SHA256_DIGEST_SIZE];
  VOID                 *Context1;
  VOID                 *Context2;
  UINTN                ContextSize;
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
  // Empty data is valid to hash
  //
  if (Size == 0) {
    //
    // Test empty hash
    //
    memset (HashResult1, 0, sizeof (HashResult1));
    Crypto->Sha256HashAll (Data, 0, HashResult1);
    return 0;
  }

  //
  // Test 1: Hash all data at once using Sha256HashAll
  //
  memset (HashResult1, 0, sizeof (HashResult1));
  Result = Crypto->Sha256HashAll (Data, Size, HashResult1);
  if (!Result) {
    // Hash operation failed, continue to next test
    return 0;
  }

  //
  // Test 2: Hash incrementally using Init/Update/Final
  //
  ContextSize = Crypto->Sha256GetContextSize ();
  if (ContextSize == 0) {
    return 0;
  }

  Context1 = malloc (ContextSize);
  if (Context1 == NULL) {
    return 0;
  }

  memset (HashResult2, 0, sizeof (HashResult2));

  Result = Crypto->Sha256Init (Context1);
  if (!Result) {
    free (Context1);
    return 0;
  }

  //
  // Feed data in chunks to test incremental hashing
  //
  Ptr       = Data;
  Remaining = Size;
  while (Remaining > 0) {
    size_t ChunkSize = (Remaining > MAX_UPDATE_SIZE) ? MAX_UPDATE_SIZE : Remaining;

    //
    // Use first byte of remaining data to vary chunk size for more coverage
    //
    if (Remaining > 1 && ChunkSize > 1) {
      ChunkSize = (Ptr[0] % ChunkSize) + 1;
    }

    Result = Crypto->Sha256Update (Context1, Ptr, ChunkSize);
    if (!Result) {
      free (Context1);
      return 0;
    }

    Ptr       += ChunkSize;
    Remaining -= ChunkSize;
  }

  Result = Crypto->Sha256Final (Context1, HashResult2);
  if (!Result) {
    free (Context1);
    return 0;
  }

  //
  // Verify both methods produce the same hash
  //
  if (memcmp (HashResult1, HashResult2, SHA256_DIGEST_SIZE) != 0) {
    //
    // Hash mismatch - this is a bug!
    //
    __builtin_trap ();
  }

  //
  // Test 3: Test context duplication
  //
  Result = Crypto->Sha256Init (Context1);
  if (!Result) {
    free (Context1);
    return 0;
  }

  //
  // Hash half the data
  //
  size_t HalfSize = Size / 2;
  if (HalfSize > 0) {
    Result = Crypto->Sha256Update (Context1, Data, HalfSize);
    if (!Result) {
      free (Context1);
      return 0;
    }
  }

  //
  // Duplicate the context
  //
  Context2 = malloc (ContextSize);
  if (Context2 == NULL) {
    free (Context1);
    return 0;
  }

  Result = Crypto->Sha256Duplicate (Context1, Context2);
  if (!Result) {
    free (Context1);
    free (Context2);
    return 0;
  }

  //
  // Hash the rest of the data on both contexts
  //
  if (Size > HalfSize) {
    Result = Crypto->Sha256Update (Context1, Data + HalfSize, Size - HalfSize);
    if (!Result) {
      free (Context1);
      free (Context2);
      return 0;
    }

    Result = Crypto->Sha256Update (Context2, Data + HalfSize, Size - HalfSize);
    if (!Result) {
      free (Context1);
      free (Context2);
      return 0;
    }
  }

  //
  // Finalize both and verify they match
  //
  memset (HashResult2, 0, sizeof (HashResult2));
  memset (HashResult3, 0, sizeof (HashResult3));

  Result = Crypto->Sha256Final (Context1, HashResult2);
  if (Result) {
    Result = Crypto->Sha256Final (Context2, HashResult3);
    if (Result) {
      if (memcmp (HashResult2, HashResult3, SHA256_DIGEST_SIZE) != 0) {
        //
        // Duplicated contexts should produce same hash
        //
        __builtin_trap ();
      }
    }
  }

  free (Context1);
  free (Context2);

  return 0;
}
