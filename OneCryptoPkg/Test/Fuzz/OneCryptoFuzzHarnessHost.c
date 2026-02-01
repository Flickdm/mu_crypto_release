/** @file
  OneCryptoFuzzHarnessHost.c

  Common fuzzing harness infrastructure for OneCrypto using host-compatible types.
  Provides initialization and cleanup routines used by all fuzzing targets.

  This implementation provides POSIX/libc implementations for the
  ONE_CRYPTO_DEPENDENCIES interface, allowing the phase-agnostic crypto
  binary to be loaded and tested without UEFI firmware.

  The loader can work in two modes:
  1. PE/COFF loading: Load the production .efi binary at runtime
  2. Direct linking: Link with OneCryptoBin compiled for host (requires CryptoEntry symbol)

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OneCryptoFuzzHarnessHost.h"
#include "PeCoffLoaderHost.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

//
// Global dependencies structure for the host environment
//
static ONE_CRYPTO_DEPENDENCIES  mHostDependencies;

//
// Global crypto protocol pointer
//
static ONE_CRYPTO_PROTOCOL  *mFuzzProtocol = NULL;
static int                   mInitialized  = 0;

//
// PE/COFF loaded image
//
static void   *mLoadedImage     = NULL;
static size_t  mLoadedImageSize = 0;

//
// Environment variable for the OneCrypto binary path
//
#define ONECRYPTO_BIN_ENV  "ONECRYPTO_BIN"

//
// Default path to look for the binary (relative to workspace)
//
static const char *DefaultBinaryPaths[] = {
  // Built by stuart_build
  "Build/OneCryptoPkg/DEBUG_GCC5/X64/OneCryptoPkg/OneCryptoBin/OneCryptoBinSupvMm/OUTPUT/OneCryptoBinSupvMm.efi",
  "Build/OneCryptoPkg/RELEASE_GCC5/X64/OneCryptoPkg/OneCryptoBin/OneCryptoBinSupvMm/OUTPUT/OneCryptoBinSupvMm.efi",
  "Build/OneCryptoPkg/DEBUG_GCC5/X64/OneCryptoPkg/OneCryptoBin/OneCryptoBinStandaloneMm/OUTPUT/OneCryptoBinStandaloneMm.efi",
  NULL
};

// =============================================================================
// Host Dependency Implementations
// =============================================================================

/**
  Host implementation of memory allocation.

  Uses standard malloc to allocate memory.

  @param[in]  AllocationSize  The number of bytes to allocate.

  @retval NULL    Allocation failed.
  @retval Other   Pointer to the allocated buffer.
**/
static
VOID *
EFIAPI
HostAllocatePool (
  UINTN  AllocationSize
  )
{
  return malloc (AllocationSize);
}

/**
  Host implementation of memory deallocation.

  Uses standard free to deallocate memory.

  @param[in]  Buffer  Pointer to the buffer to free.
**/
static
VOID
EFIAPI
HostFreePool (
  VOID  *Buffer
  )
{
  if (Buffer != NULL) {
    free (Buffer);
  }
}

/**
  Host implementation of debug printing.

  Note: Variadic functions with ms_abi are problematic due to different
  va_list handling between ABIs. For fuzzing purposes, we simply discard
  debug output since performance is critical.

  @param[in]  ErrorLevel  The debug message severity level.
  @param[in]  Format      Printf-style format string.
  @param[in]  ...         Variable arguments (ignored).
**/
static
VOID
EFIAPI
HostDebugPrint (
  UINTN        ErrorLevel,
  CONST CHAR8  *Format,
  ...
  )
{
  //
  // Variadic functions with ms_abi calling convention are complex.
  // For fuzzing, we simply discard debug output for performance.
  // In a debug build, you could enable this with special handling.
  //
  (void)ErrorLevel;
  (void)Format;
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
static
EFI_STATUS
EFIAPI
HostGetTime (
  EFI_TIME               *Time,
  EFI_TIME_CAPABILITIES  *Capabilities
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

  memset (Time, 0, sizeof (EFI_TIME));
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

  Reads from /dev/urandom or uses rand() as fallback.

  @param[out]  Rand  Pointer to storage for the 64-bit random number.

  @retval TRUE   Random number was generated successfully.
  @retval FALSE  Failed to generate random number.
**/
static
BOOLEAN
EFIAPI
HostGetRandomNumber64 (
  UINT64  *Rand
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
static
void
InitializeHostDependencies (
  void
  )
{
  memset (&mHostDependencies, 0, sizeof (mHostDependencies));

  mHostDependencies.Major             = ONE_CRYPTO_DEPENDENCIES_VERSION_MAJOR;
  mHostDependencies.Minor             = ONE_CRYPTO_DEPENDENCIES_VERSION_MINOR;
  mHostDependencies.Reserved          = 0;
  mHostDependencies.AllocatePool      = HostAllocatePool;
  mHostDependencies.FreePool          = HostFreePool;
  mHostDependencies.DebugPrint        = HostDebugPrint;
  mHostDependencies.GetTime           = HostGetTime;
  mHostDependencies.GetRandomNumber64 = HostGetRandomNumber64;
}

// =============================================================================
// Public API
// =============================================================================

/**
  Find the OneCrypto binary to load.

  Checks ONECRYPTO_BIN environment variable first, then tries default paths.

  @return Path to the binary, or NULL if not found.
**/
static
const char *
FindOneCryptoBinary (
  void
  )
{
  const char  *EnvPath;
  const char  *WorkspaceEnv;
  static char  FullPath[4096];
  int          i;
  FILE        *TestFile;

  //
  // Check environment variable first
  //
  EnvPath = getenv (ONECRYPTO_BIN_ENV);
  if (EnvPath != NULL) {
    TestFile = fopen (EnvPath, "rb");
    if (TestFile != NULL) {
      fclose (TestFile);
      return EnvPath;
    }
    fprintf (stderr, "FuzzInit: ONECRYPTO_BIN set but file not found: %s\n", EnvPath);
  }

  //
  // Try default paths relative to workspace
  //
  WorkspaceEnv = getenv ("WORKSPACE");
  if (WorkspaceEnv == NULL) {
    // Try to find it relative to current directory
    WorkspaceEnv = "../../..";
  }

  for (i = 0; DefaultBinaryPaths[i] != NULL; i++) {
    snprintf (FullPath, sizeof (FullPath), "%s/%s", WorkspaceEnv, DefaultBinaryPaths[i]);
    TestFile = fopen (FullPath, "rb");
    if (TestFile != NULL) {
      fclose (TestFile);
      fprintf (stderr, "FuzzInit: Found OneCrypto binary at: %s\n", FullPath);
      return FullPath;
    }
  }

  return NULL;
}

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
  )
{
  EFI_STATUS    Status;
  UINT32        CryptoSize;
  VOID          *CryptoProtocol;
  CRYPTO_ENTRY  EntryPoint;
  const char    *BinaryPath;

  if (mInitialized) {
    return 0;
  }

  //
  // Initialize host dependencies
  //
  InitializeHostDependencies ();

  //
  // Load PE/COFF binary to get CryptoEntry
  //
  BinaryPath = FindOneCryptoBinary ();
  if (BinaryPath == NULL) {
    fprintf (stderr, "FuzzInit: Cannot find OneCrypto binary.\n");
    fprintf (stderr, "FuzzInit: Set ONECRYPTO_BIN environment variable or build the project first.\n");
    fprintf (stderr, "FuzzInit: Expected paths:\n");
    for (int i = 0; DefaultBinaryPaths[i] != NULL; i++) {
      fprintf (stderr, "  - %s\n", DefaultBinaryPaths[i]);
    }
    return -1;
  }

  if (PeCoffLoadImage (BinaryPath, &mLoadedImage, &mLoadedImageSize) != 0) {
    fprintf (stderr, "FuzzInit: Failed to load PE/COFF image\n");
    return -1;
  }

  EntryPoint = (CRYPTO_ENTRY)PeCoffFindExport (mLoadedImage, "CryptoEntry");
  if (EntryPoint == NULL) {
    fprintf (stderr, "FuzzInit: CryptoEntry export not found in image\n");
    PeCoffUnloadImage (mLoadedImage);
    mLoadedImage = NULL;
    return -1;
  }

  //
  // Query the size needed for the crypto protocol
  //
  CryptoSize = 0;
  Status     = EntryPoint (&mHostDependencies, NULL, &CryptoSize);
  if ((Status != EFI_BUFFER_TOO_SMALL) || (CryptoSize == 0)) {
    fprintf (stderr, "FuzzInit: Failed to query crypto protocol size: 0x%lx\n", (unsigned long)Status);
    return -1;
  }

  fprintf (stderr, "FuzzInit: Crypto protocol size: %u bytes\n", CryptoSize);

  //
  // Allocate memory for the crypto protocol
  //
  CryptoProtocol = malloc (CryptoSize);
  if (CryptoProtocol == NULL) {
    fprintf (stderr, "FuzzInit: Failed to allocate memory for crypto protocol\n");
    return -1;
  }

  //
  // Initialize the crypto protocol
  //
  Status = EntryPoint (&mHostDependencies, &CryptoProtocol, &CryptoSize);
  if (EFI_ERROR (Status)) {
    fprintf (stderr, "FuzzInit: CryptoEntry failed: 0x%lx\n", (unsigned long)Status);
    free (CryptoProtocol);
    return -1;
  }

  mFuzzProtocol = (ONE_CRYPTO_PROTOCOL *)CryptoProtocol;

  fprintf (
    stderr,
    "FuzzInit: Crypto protocol initialized (version %u.%u)\n",
    (unsigned)mFuzzProtocol->Major,
    (unsigned)mFuzzProtocol->Minor
    );

  mInitialized = 1;
  return 0;
}

/**
  Cleanup the fuzzing harness.

  This should be called when the fuzzer is done to free resources.
**/
void
OneCryptoFuzzCleanup (
  void
  )
{
  if (!mInitialized) {
    return;
  }

  if (mFuzzProtocol != NULL) {
    free (mFuzzProtocol);
    mFuzzProtocol = NULL;
  }

  if (mLoadedImage != NULL) {
    PeCoffUnloadImage (mLoadedImage);
    mLoadedImage     = NULL;
    mLoadedImageSize = 0;
  }

  mInitialized = 0;
}

/**
  Get the crypto protocol for fuzzing.

  @return Pointer to ONE_CRYPTO_PROTOCOL or NULL if not initialized.
**/
ONE_CRYPTO_PROTOCOL *
OneCryptoFuzzGetProtocol (
  void
  )
{
  return mFuzzProtocol;
}
