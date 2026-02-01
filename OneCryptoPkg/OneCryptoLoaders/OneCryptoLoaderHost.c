/** @file
  OneCryptoLoaderHost.c

  Host-based loader for OneCrypto that enables fuzzing and testing on the
  host system. This loader provides POSIX/libc implementations for the
  ONE_CRYPTO_DEPENDENCIES interface, allowing the phase-agnostic crypto
  binary to be loaded and tested without UEFI firmware.

  The design philosophy:
  - The OneCrypto binary is phase-agnostic and only depends on the
    ONE_CRYPTO_DEPENDENCIES interface
  - This host loader provides native implementations for all dependencies
  - The crypto protocol can then be used by fuzzing harnesses

  Usage:
  1. Build OneCryptoBinHost which exports CryptoEntry
  2. This loader initializes dependencies and calls CryptoEntry
  3. Fuzzing harnesses can then exercise the crypto protocol functions

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Private/OneCryptoDependencySupport.h>
#include <Protocol/OneCrypto.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

//
// Global dependencies structure for the host environment
//
STATIC ONE_CRYPTO_DEPENDENCIES  mHostDependencies;

//
// Global crypto protocol pointer
//
STATIC ONE_CRYPTO_PROTOCOL  *mOneCryptoProtocol = NULL;

//
// Forward declaration of the CryptoEntry function exported by OneCryptoBinHost
//
extern CRYPTO_ENTRY  CryptoEntry;

/**
  Host implementation of memory allocation.

  Uses standard malloc to allocate memory.

  @param[in]  AllocationSize  The number of bytes to allocate.

  @retval NULL    Allocation failed.
  @retval Other   Pointer to the allocated buffer.
**/
STATIC
VOID *
EFIAPI
HostAllocatePool (
  IN UINTN  AllocationSize
  )
{
  return malloc (AllocationSize);
}

/**
  Host implementation of memory deallocation.

  Uses standard free to deallocate memory.

  @param[in]  Buffer  Pointer to the buffer to free.
**/
STATIC
VOID
EFIAPI
HostFreePool (
  IN VOID  *Buffer
  )
{
  if (Buffer != NULL) {
    free (Buffer);
  }
}

/**
  Host implementation of debug printing.

  Outputs debug messages to stderr with the error level prefix.

  @param[in]  ErrorLevel  The debug message severity level.
  @param[in]  Format      Printf-style format string.
  @param[in]  ...         Variable arguments.
**/
STATIC
VOID
EFIAPI
HostDebugPrint (
  IN UINTN        ErrorLevel,
  IN CONST CHAR8  *Format,
  ...
  )
{
  VA_LIST  Args;
  CHAR8    Buffer[512];

  VA_START (Args, Format);
  AsciiVSPrint (Buffer, sizeof (Buffer), Format, Args);
  VA_END (Args);

  //
  // Print to stderr with level indicator
  //
  fprintf (stderr, "[0x%08lx] %s", (unsigned long)ErrorLevel, Buffer);
}

/**
  Host implementation of GetTime.

  Uses the standard C library time functions to get the current time
  and converts it to EFI_TIME format.

  @param[out]  Time          Pointer to storage for the current time.
  @param[out]  Capabilities  Optional pointer to time capabilities (ignored).

  @retval EFI_SUCCESS           Time was retrieved successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
**/
STATIC
EFI_STATUS
EFIAPI
HostGetTime (
  OUT EFI_TIME               *Time,
  OUT EFI_TIME_CAPABILITIES  *Capabilities OPTIONAL
  )
{
  time_t     RawTime;
  struct tm  *TimeInfo;

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  time (&RawTime);
  TimeInfo = localtime (&RawTime);

  if (TimeInfo == NULL) {
    return EFI_DEVICE_ERROR;
  }

  ZeroMem (Time, sizeof (EFI_TIME));
  Time->Year       = (UINT16)(TimeInfo->tm_year + 1900);
  Time->Month      = (UINT8)(TimeInfo->tm_mon + 1);
  Time->Day        = (UINT8)TimeInfo->tm_mday;
  Time->Hour       = (UINT8)TimeInfo->tm_hour;
  Time->Minute     = (UINT8)TimeInfo->tm_min;
  Time->Second     = (UINT8)TimeInfo->tm_sec;
  Time->Nanosecond = 0;
  Time->TimeZone   = EFI_UNSPECIFIED_TIMEZONE;
  Time->Daylight   = 0;

  if (Capabilities != NULL) {
    Capabilities->Resolution = 1;
    Capabilities->Accuracy   = 50000000;  // 50 ms
    Capabilities->SetsToZero = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  Host implementation of random number generation.

  For fuzzing purposes, we provide deterministic "random" numbers
  based on the fuzzer's input when available, or use a PRNG seeded
  from /dev/urandom otherwise.

  @param[out]  Rand  Pointer to storage for the 64-bit random number.

  @retval TRUE   Random number was generated successfully.
  @retval FALSE  Failed to generate random number.
**/
STATIC
BOOLEAN
EFIAPI
HostGetRandomNumber64 (
  OUT UINT64  *Rand
  )
{
  FILE  *Urandom;

  if (Rand == NULL) {
    return FALSE;
  }

  //
  // Try to read from /dev/urandom for better entropy
  //
  Urandom = fopen ("/dev/urandom", "rb");
  if (Urandom != NULL) {
    size_t  BytesRead = fread (Rand, 1, sizeof (UINT64), Urandom);
    fclose (Urandom);
    if (BytesRead == sizeof (UINT64)) {
      return TRUE;
    }
  }

  //
  // Fallback to rand() if /dev/urandom not available
  //
  *Rand = ((UINT64)rand () << 32) | (UINT64)rand ();
  return TRUE;
}

/**
  Initialize the host dependencies structure.

  Sets up the ONE_CRYPTO_DEPENDENCIES structure with host-native
  implementations for all required services.
**/
STATIC
VOID
InitializeHostDependencies (
  VOID
  )
{
  ZeroMem (&mHostDependencies, sizeof (mHostDependencies));

  mHostDependencies.Major             = ONE_CRYPTO_DEPENDENCIES_VERSION_MAJOR;
  mHostDependencies.Minor             = ONE_CRYPTO_DEPENDENCIES_VERSION_MINOR;
  mHostDependencies.Reserved          = 0;
  mHostDependencies.AllocatePool      = HostAllocatePool;
  mHostDependencies.FreePool          = HostFreePool;
  mHostDependencies.DebugPrint        = HostDebugPrint;
  mHostDependencies.GetTime           = HostGetTime;
  mHostDependencies.GetRandomNumber64 = HostGetRandomNumber64;
}

/**
  Get the initialized crypto protocol.

  Returns the ONE_CRYPTO_PROTOCOL pointer that was initialized by
  OneCryptoHostLoaderInit. Returns NULL if not yet initialized.

  @return Pointer to ONE_CRYPTO_PROTOCOL or NULL if not initialized.
**/
ONE_CRYPTO_PROTOCOL *
EFIAPI
OneCryptoHostGetProtocol (
  VOID
  )
{
  return mOneCryptoProtocol;
}

/**
  Initialize the OneCrypto host loader.

  This function sets up the host dependencies and calls the CryptoEntry
  function to initialize the crypto protocol. After successful initialization,
  the crypto protocol is available via OneCryptoHostGetProtocol().

  @retval EFI_SUCCESS          Crypto protocol initialized successfully.
  @retval EFI_OUT_OF_RESOURCES Failed to allocate memory.
  @retval Other                Error from CryptoEntry.
**/
EFI_STATUS
EFIAPI
OneCryptoHostLoaderInit (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT32      CryptoSize;
  VOID        *CryptoProtocol;

  //
  // Initialize host dependencies
  //
  InitializeHostDependencies ();

  //
  // Query the size needed for the crypto protocol
  //
  CryptoSize = 0;
  Status     = CryptoEntry (&mHostDependencies, NULL, &CryptoSize);
  if ((Status != EFI_BUFFER_TOO_SMALL) || (CryptoSize == 0)) {
    fprintf (stderr, "OneCryptoHostLoader: Failed to query crypto protocol size: 0x%lx\n", (unsigned long)Status);
    return EFI_PROTOCOL_ERROR;
  }

  fprintf (stderr, "OneCryptoHostLoader: Crypto protocol size: %u bytes\n", CryptoSize);

  //
  // Allocate memory for the crypto protocol
  //
  CryptoProtocol = malloc (CryptoSize);
  if (CryptoProtocol == NULL) {
    fprintf (stderr, "OneCryptoHostLoader: Failed to allocate memory for crypto protocol\n");
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the crypto protocol
  //
  Status = CryptoEntry (&mHostDependencies, &CryptoProtocol, &CryptoSize);
  if (EFI_ERROR (Status)) {
    fprintf (stderr, "OneCryptoHostLoader: CryptoEntry failed: 0x%lx\n", (unsigned long)Status);
    free (CryptoProtocol);
    return Status;
  }

  mOneCryptoProtocol = (ONE_CRYPTO_PROTOCOL *)CryptoProtocol;

  fprintf (
    stderr,
    "OneCryptoHostLoader: Crypto protocol initialized (version %llu.%llu)\n",
    (unsigned long long)mOneCryptoProtocol->Major,
    (unsigned long long)mOneCryptoProtocol->Minor
    );

  return EFI_SUCCESS;
}

/**
  Cleanup the OneCrypto host loader.

  Frees any resources allocated during initialization.
**/
VOID
EFIAPI
OneCryptoHostLoaderCleanup (
  VOID
  )
{
  if (mOneCryptoProtocol != NULL) {
    free (mOneCryptoProtocol);
    mOneCryptoProtocol = NULL;
  }
}

/**
  Get pointer to the host dependencies structure.

  This is useful for tests that need to modify or inspect the dependencies.

  @return Pointer to the ONE_CRYPTO_DEPENDENCIES structure.
**/
ONE_CRYPTO_DEPENDENCIES *
EFIAPI
OneCryptoHostGetDependencies (
  VOID
  )
{
  return &mHostDependencies;
}
