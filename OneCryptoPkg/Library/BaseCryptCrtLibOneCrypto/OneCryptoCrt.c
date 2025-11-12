/** @file
  C Run-Time Libraries (CRT) implementations
  These either call the shared implementations or implement the logic itself
  if the implementation is simple enough that the code may be throughly vetted
  to ensure safety.

  Copyright (c) Microsoft Corporation. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Private/OneCryptoDependencySupport.h>
#include <CrtLibSupport.h>

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
AllocatePool (
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
AllocateZeroPool (
  IN UINTN  AllocationSize
  )
{
  VOID  *Buffer;

  Buffer = AllocatePool (AllocationSize);
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
FreePool (
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
GetTime (
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
GetRandomNumber64 (
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
