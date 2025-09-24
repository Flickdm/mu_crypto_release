/** @file
  SharedCryptoDependencySupport.h

  This file contains the definitions and constants used in the shared cryptographic library that
  are shared across different headers.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef SHARED_DEPENDENCY_SUPPORT_H
#define SHARED_DEPENDENCY_SUPPORT_H

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

//
// FILE_GUID(76ABA88D-9D16-49A2-AA3A-DB6112FAC5CC) of SharedCryptoMmBin.inf
//
#define SHARED_FILE_GUID  { 0x76ABA88D, 0x9D16, 0x49A2, { 0xAA, 0x3A, 0xDB, 0x61, 0x12, 0xFA, 0xC5, 0xCC } }

//
// The name of the exported function
//
#define CONSTRUCTOR_NAME  "Constructor"

/*
  @typedef GET_VERSION

  @brief Defines a function pointer type for a function that returns the version.
  @return UINT64 The version number.
*/
typedef UINT64 (EFIAPI *GET_VERSION)(
  VOID
  );

typedef VOID *(*ALLOCATE_POOL)(
  UINTN  AllocationSize
  );

typedef VOID (*FREE_POOL)(
  VOID  *Buffer
  );

typedef VOID (*ASSERT_T)(
  BOOLEAN  Expression
  );

typedef
VOID
(EFIAPI *DEBUG_PRINT)(
  IN UINTN        ErrorLevel,
  IN CONST CHAR8  *Format,
  ...
  );

typedef EFI_STATUS (EFIAPI *GET_TIME)(
  OUT  EFI_TIME               *Time,
  OUT  EFI_TIME_CAPABILITIES  *Capabilities OPTIONAL
  );

typedef
BOOLEAN
(EFIAPI *GET_RANDOM_NUMBER_64)(
  OUT UINT64  *Rand
  );

// Structure to hold function pointers
typedef struct  _SHARED_DEPENDENCIES {
  GET_VERSION             GetVersion;
  ALLOCATE_POOL           AllocatePool;
  FREE_POOL               FreePool;
  ASSERT_T                ASSERT;
  GET_TIME                GetTime;
  DEBUG_PRINT             DebugPrint;
  GET_RANDOM_NUMBER_64    GetRandomNumber64;
} SHARED_DEPENDENCIES;

SHARED_DEPENDENCIES  *gSharedDepends;

///////////////////////////////////////////////////////////////////////////////
/// Exported Constructor
///////////////////////////////////////////////////////////////////////////////

#define SHARED_CRYPTO_MM_CONSTRUCTOR_PROTOCOL_SIGNATURE SIGNATURE_32('S', 'C', 'M', 'C')

/**
 * @typedef CONSTRUCTOR
 * @brief Defines a function pointer type for a constructor function.
 *
 * @param Imports A pointer to an IMPORTS structure.
 *
 * @return EFI_STATUS The status of the constructor function.
 */

typedef EFI_STATUS (EFIAPI *CONSTRUCTOR)(
  IN SHARED_DEPENDENCIES *Depends,
  OUT VOID *RequestedCrypto
  );

//
// Protocol Definition
//
typedef struct _SHARED_CRYPTO_MM_CONSTRUCTOR_PROTOCOL {
  UINT32         Signature;
  UINT32         Version;
  CONSTRUCTOR    Constructor;
} SHARED_CRYPTO_MM_CONSTRUCTOR_PROTOCOL;

#endif // SHARED_DEPENDENCY_SUPPORT_H
