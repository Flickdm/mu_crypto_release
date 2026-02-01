/** @file
  HostUefiCompat.h

  UEFI type compatibility header for host-based fuzzing.

  This header provides UEFI-compatible type definitions that work on a Linux host
  while maintaining ABI compatibility with PE/COFF loaded UEFI binaries.

  Include this header BEFORE any UEFI headers to override their definitions.

  Key features:
  - Defines EFIAPI with ms_abi for correct calling convention
  - Provides all basic UEFI types (UINT8, BOOLEAN, EFI_STATUS, etc.)
  - Guards prevent real UEFI headers from redefining types

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef HOST_UEFI_COMPAT_H_
#define HOST_UEFI_COMPAT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//
// Prevent real UEFI headers from being included
// These guards match the guards in EDK2 headers
//
#define __BASE_H__
#define __UEFI_BASETYPE_H__
#define __PI_BASE_H__
#define __UEFI_MULTIPHASE_H__

//
// Basic type definitions matching UEFI
//
typedef uint8_t   UINT8;
typedef int8_t    INT8;
typedef uint16_t  UINT16;
typedef int16_t   INT16;
typedef uint32_t  UINT32;
typedef int32_t   INT32;
typedef uint64_t  UINT64;
typedef int64_t   INT64;
typedef size_t    UINTN;
typedef intptr_t  INTN;
typedef char      CHAR8;
typedef uint16_t  CHAR16;  // UEFI uses 16-bit chars, not wchar_t
typedef uint8_t   BOOLEAN;
typedef void      VOID;

#define TRUE   ((BOOLEAN)(1==1))
#define FALSE  ((BOOLEAN)(0==1))

#ifndef NULL
#define NULL   ((VOID *)0)
#endif

//
// Modifier keywords
//
#define IN
#define OUT
#define OPTIONAL
#define CONST  const
#define STATIC static

//
// Calling convention for UEFI functions
// UEFI uses Microsoft x64 ABI even when compiled with GCC on Linux
// MS ABI: args in RCX, RDX, R8, R9
// SysV ABI: args in RDI, RSI, RDX, RCX, R8, R9
//
#if defined(__x86_64__) || defined(_M_X64)
#define EFIAPI  __attribute__((ms_abi))
#else
#define EFIAPI
#endif

//
// Return status type and macros
//
typedef UINTN RETURN_STATUS;
typedef UINTN EFI_STATUS;

#define MAX_BIT      (1ULL << 63)
#define ENCODE_ERROR(StatusCode)  ((RETURN_STATUS)(MAX_BIT | (StatusCode)))
#define RETURN_ERROR(StatusCode)  (((INTN)(RETURN_STATUS)(StatusCode)) < 0)
#define EFI_ERROR(A)              RETURN_ERROR(A)

//
// Common return status codes
//
#define RETURN_SUCCESS               0
#define RETURN_LOAD_ERROR            ENCODE_ERROR(1)
#define RETURN_INVALID_PARAMETER     ENCODE_ERROR(2)
#define RETURN_UNSUPPORTED           ENCODE_ERROR(3)
#define RETURN_BAD_BUFFER_SIZE       ENCODE_ERROR(4)
#define RETURN_BUFFER_TOO_SMALL      ENCODE_ERROR(5)
#define RETURN_NOT_READY             ENCODE_ERROR(6)
#define RETURN_DEVICE_ERROR          ENCODE_ERROR(7)
#define RETURN_WRITE_PROTECTED       ENCODE_ERROR(8)
#define RETURN_OUT_OF_RESOURCES      ENCODE_ERROR(9)

#define EFI_SUCCESS               RETURN_SUCCESS
#define EFI_LOAD_ERROR            RETURN_LOAD_ERROR
#define EFI_INVALID_PARAMETER     RETURN_INVALID_PARAMETER
#define EFI_UNSUPPORTED           RETURN_UNSUPPORTED
#define EFI_BAD_BUFFER_SIZE       RETURN_BAD_BUFFER_SIZE
#define EFI_BUFFER_TOO_SMALL      RETURN_BUFFER_TOO_SMALL
#define EFI_NOT_READY             RETURN_NOT_READY
#define EFI_DEVICE_ERROR          RETURN_DEVICE_ERROR
#define EFI_WRITE_PROTECTED       RETURN_WRITE_PROTECTED
#define EFI_OUT_OF_RESOURCES      RETURN_OUT_OF_RESOURCES

//
// EFI_GUID type
//
typedef struct {
  UINT32  Data1;
  UINT16  Data2;
  UINT16  Data3;
  UINT8   Data4[8];
} GUID;

typedef GUID EFI_GUID;

//
// EFI_TIME and related
//
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
  UINT8   Pad1;
  UINT32  Nanosecond;
  INT16   TimeZone;
  UINT8   Daylight;
  UINT8   Pad2;
} EFI_TIME;

#define EFI_UNSPECIFIED_TIMEZONE  0x07FF

typedef struct {
  UINT32   Resolution;
  UINT32   Accuracy;
  BOOLEAN  SetsToZero;
} EFI_TIME_CAPABILITIES;

//
// Utility macros
//
#define SIGNATURE_16(A, B)        ((UINT16)((A) | ((B) << 8)))
#define SIGNATURE_32(A, B, C, D)  ((UINT32)((A) | ((B) << 8) | ((C) << 16) | ((D) << 24)))

//
// Memory functions (map to libc)
//
#include <string.h>
#define ZeroMem(Dest, Size)        memset((Dest), 0, (Size))
#define SetMem(Dest, Size, Val)    memset((Dest), (Val), (Size))
#define CopyMem(Dest, Src, Size)   memcpy((Dest), (Src), (Size))
#define CompareMem(B1, B2, Size)   memcmp((B1), (B2), (Size))

//
// Stub out DEBUG macro (commonly used in UEFI code)
//
#define DEBUG(Expression)
#define ASSERT(Expression)

#endif // HOST_UEFI_COMPAT_H_
