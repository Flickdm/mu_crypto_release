/** @file
  BaseCryptCrtLib.c
  
  Implementation of BaseCryptCrtLib that manages CRT dependencies for
  cryptographic operations.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseCryptCrtLib.h>
#include <Library/DebugLib.h>
#include <Private/OneCryptoDependencySupport.h>

//
// Static pointer to hold the dependencies
//
STATIC ONE_CRYPTO_DEPENDENCIES  *mCryptoDependencies = NULL;

/**
  Initialize the OneCrypto CRT library with the provided dependencies.
  
  This function stores a pointer to the dependency structure which provides
  implementations for memory allocation, time services, random number generation,
  and debugging functions.
  
  @param[in]  Dependencies  Pointer to ONE_CRYPTO_DEPENDENCIES structure containing
                            function pointers for required services.
  
  @retval EFI_SUCCESS           Dependencies were set successfully.
  @retval EFI_INVALID_PARAMETER Dependencies is NULL.
**/
EFI_STATUS
EFIAPI
BaseCryptCrtSetup (
  IN ONE_CRYPTO_DEPENDENCIES  *Dependencies
  )
{
  if (Dependencies == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  mCryptoDependencies = Dependencies;
  return EFI_SUCCESS;
}

/**
  Get the current CRT dependencies.
  
  Internal function to retrieve the dependencies pointer for use by
  other CRT functions.
  
  @return Pointer to ONE_CRYPTO_DEPENDENCIES structure, or NULL if not initialized.
**/
ONE_CRYPTO_DEPENDENCIES *
EFIAPI
GetCryptoDependencies (
  VOID
  )
{
  return mCryptoDependencies;
}


/**
  Allocates a buffer of a specified size from the pool.

  This function allocates a buffer of size AllocationSize from the pool. If the
  global shared dependencies or the AllocatePool function pointer within it is
  NULL, the function returns NULL.

  @param[in]  AllocationSize  The number of bytes to allocate.

  @retval  NULL    If gOneCryptoDepends or gOneCryptoDepends->AllocatePool is NULL.
  @retval  Others  A pointer to the allocated buffer.
**/
VOID *
EFIAPI
BaseCryptAllocatePool (
  IN UINTN  AllocationSize
  )
{
  ONE_CRYPTO_DEPENDENCIES  *Dependencies;

  Dependencies = GetCryptoDependencies ();
  if ((Dependencies == NULL) || (Dependencies->AllocatePool == NULL)) {
    ASSERT (Dependencies != NULL);
    ASSERT (Dependencies->AllocatePool != NULL);
    return NULL;
  }

  return Dependencies->AllocatePool (AllocationSize);
}

/**
  Allocates and zeros a buffer of a specified size from the pool.

  This function allocates a buffer of size AllocationSize from the pool and then
  zeros the entire allocated buffer. If the allocation fails, the function returns NULL.

  @param[in]  AllocationSize  The number of bytes to allocate and zero.

  @retval  NULL    If the allocation fails, or if AllocationSize is 0.
  @retval  Others  A pointer to the allocated and zeroed buffer.
**/
VOID *
EFIAPI
BaseCryptAllocateZeroPool (
  IN UINTN  AllocationSize
  )
{
  VOID  *Buffer;

  Buffer = BaseCryptAllocatePool (AllocationSize);
  if (Buffer != NULL) {
    SetMem (Buffer, AllocationSize, 0);
  }

  return Buffer;
}

/**
  Frees a pool of memory.

  This function checks if the global shared dependencies and its FreePool function
  pointer are not NULL. If both are valid, it calls the FreePool function to free
  the memory pool pointed to by Buffer.

  @param[in]  Buffer  Pointer to the memory pool to be freed.
**/
VOID
EFIAPI
BaseCryptFreePool (
  IN VOID  *Buffer
  )
{
  ONE_CRYPTO_DEPENDENCIES  *Dependencies;

  Dependencies = GetCryptoDependencies ();
  if ((Dependencies == NULL) || (Dependencies->FreePool == NULL)) {
    ASSERT (Dependencies != NULL);
    ASSERT (Dependencies->FreePool != NULL);
    return;
  }

  Dependencies->FreePool (Buffer);
}

/**
  Retrieves the current time and date information, and the time-keeping capabilities of the hardware platform.

  @param[out] Time          A pointer to storage to receive a snapshot of the current time.
  @param[out] Capabilities  An optional pointer to a buffer to receive the real time clock device's capabilities.

  @retval EFI_SUCCESS       The operation completed successfully.
  @retval EFI_UNSUPPORTED   The operation is not supported.
**/
EFI_STATUS
EFIAPI
BaseCryptGetTime (
  OUT  EFI_TIME               *Time,
  OUT  EFI_TIME_CAPABILITIES  *Capabilities OPTIONAL
  )
{
  ONE_CRYPTO_DEPENDENCIES  *Dependencies;

  Dependencies = GetCryptoDependencies ();
  if ((Dependencies == NULL) || (Dependencies->GetTime == NULL)) {
    ASSERT (Dependencies != NULL);
    ASSERT (Dependencies->GetTime != NULL);
    return EFI_UNSUPPORTED;
  }

  return Dependencies->GetTime (Time, Capabilities);
}

/**
  Generates a 64-bit random number.

  This function attempts to generate a 64-bit random number and store it in the
  location pointed to by Rand. If the shared dependency or the GetRandomNumber64
  function pointer is NULL, the function returns FALSE.

  @param[out] Rand  Pointer to the buffer to store the 64-bit random number.

  @retval TRUE   The 64-bit random number was generated successfully.
  @retval FALSE  The shared dependency or the GetRandomNumber64 function pointer
                 is NULL, and the random number could not be generated.
**/
BOOLEAN
EFIAPI
BaseCryptGetRandomNumber64 (
  OUT UINT64  *Rand
  )
{
  ONE_CRYPTO_DEPENDENCIES  *Dependencies;

  Dependencies = GetCryptoDependencies ();
  if ((Dependencies == NULL) || (Dependencies->GetRandomNumber64 == NULL)) {
    ASSERT (Dependencies != NULL);
    ASSERT (Dependencies->GetRandomNumber64 != NULL);
    return FALSE;
  }

  return Dependencies->GetRandomNumber64 (Rand);
}

