/** @file
  OneCryptoFuzzHarnessHost.h

  Header file for OneCrypto fuzzing harness infrastructure using host-compatible types.
  Provides common types, macros, and utilities for writing fuzzing harnesses.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef ONE_CRYPTO_FUZZ_HARNESS_HOST_H_
#define ONE_CRYPTO_FUZZ_HARNESS_HOST_H_

//
// Include host-compatible UEFI types BEFORE the real OneCrypto.h
// This provides ms_abi calling convention and compatible type definitions
//
#include "HostUefiCompat.h"

//
// Include the real OneCrypto protocol header
// Our Library/BaseCryptLib.h stub satisfies its dependencies
//
#include "../../../MU_BASECORE/CryptoPkg/Include/Protocol/OneCrypto.h"

//
// Include the real OneCrypto dependency support header
//
#include "../../Include/Private/OneCryptoDependencySupport.h"

#include <stdint.h>
#include <stddef.h>

/**
  Macro to ensure minimum input size for fuzzing.
  Returns early if the input is too small.
**/
#define FUZZ_REQUIRE_MIN_SIZE(Data, Size, MinSize)  \
  do {                                               \
    if ((Size) < (MinSize)) {                        \
      return 0;                                      \
    }                                                \
  } while (0)

/**
  Macro to safely read a value from fuzzer input.
  Advances the data pointer and decrements size.
**/
#define FUZZ_READ_VALUE(Data, Size, Type, Var)      \
  do {                                               \
    if ((Size) < sizeof (Type)) {                    \
      return 0;                                      \
    }                                                \
    (Var) = *(const Type *)(Data);                   \
    (Data) = (const uint8_t *)(Data) + sizeof (Type);\
    (Size) -= sizeof (Type);                         \
  } while (0)

/**
  Macro to read a buffer from fuzzer input with a size limit.
**/
#define FUZZ_READ_BUFFER(Data, Size, BufPtr, BufSize, MaxSize)  \
  do {                                                           \
    if ((Size) < sizeof (uint32_t)) {                            \
      return 0;                                                  \
    }                                                            \
    (BufSize) = *(const uint32_t *)(Data);                       \
    (Data) = (const uint8_t *)(Data) + sizeof (uint32_t);        \
    (Size) -= sizeof (uint32_t);                                 \
    if ((BufSize) > (MaxSize)) {                                 \
      (BufSize) = (MaxSize);                                     \
    }                                                            \
    if ((Size) < (BufSize)) {                                    \
      (BufSize) = (Size);                                        \
    }                                                            \
    (BufPtr) = (Data);                                           \
    (Data) = (const uint8_t *)(Data) + (BufSize);                \
    (Size) -= (BufSize);                                         \
  } while (0)

/**
  Initialize the fuzzing harness.

  This should be called once at the start of the fuzzer to set up
  the crypto protocol.

  @retval 0   Success
  @retval -1  Initialization failed
**/
int
OneCryptoFuzzInit (
  void
  );

/**
  Cleanup the fuzzing harness.

  This should be called when the fuzzer is done to free resources.
**/
void
OneCryptoFuzzCleanup (
  void
  );

/**
  Get the crypto protocol for fuzzing.

  @return Pointer to ONE_CRYPTO_PROTOCOL or NULL if not initialized.
**/
ONE_CRYPTO_PROTOCOL *
OneCryptoFuzzGetProtocol (
  void
  );

#endif // ONE_CRYPTO_FUZZ_HARNESS_HOST_H_
