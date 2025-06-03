#ifndef SHARED_DEPENDENCY_SUPPORT_H
#define SHARED_DEPENDENCY_SUPPORT_H

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>


//
// FILE_GUID(76ABA88D-9D16-49A2-AA3A-DB6112FAC5CB) of SharedCryptoBin.inf
//
#define SHARED_FILE_GUID  { 0x76ABA88D, 0x9D16, 0x49A2, { 0xAA, 0x3A, 0xDB, 0x61, 0x12, 0xFA, 0xC5, 0xCB } }

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

/*

*/
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

#endif // SHARED_DEPENDENCY_SUPPORT_H
