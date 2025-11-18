/** @file
  BaseCryptCrtLib.h
  
  Library class for base crypto CRT support. This library provides CRT functionality
  for cryptographic operations by managing dependency injection for memory allocation,
  time services, random number generation, and debugging.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef BASE_CRYPT_CRT_LIB_H_
#define BASE_CRYPT_CRT_LIB_H_

#include <Uefi.h>
#include <Library/PrintLib.h>
#include <Private/OneCryptoDependencySupport.h> // Dependency structure

/**
  Initialize the OneCrypto CRT library with the provided dependencies.
  
  This function must be called before using any CRT functions provided by this
  library. It stores a pointer to the dependency structure which provides
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
  );

/**
  Get the current OneCrypto dependencies.
  
  This function retrieves the pointer to the dependency structure that was
  previously set using BaseCryptCrtSetup(). It is used internally by CRT
  functions to access the dependency implementations.
  
  @return Pointer to ONE_CRYPTO_DEPENDENCIES structure, or NULL if not initialized.
**/
ONE_CRYPTO_DEPENDENCIES *
EFIAPI
GetCryptoDependencies (
  VOID
  );

/**
  Allocates a buffer of type EfiBootServicesData.

  Allocates the number bytes specified by AllocationSize of type EfiBootServicesData and returns a
  pointer to the allocated buffer.  If AllocationSize is 0, then a valid buffer of 0 size is
  returned.  If there is not enough memory remaining to satisfy the request, then NULL is returned.

  @param  AllocationSize        The number of bytes to allocate.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
BaseCryptAllocatePool (
  IN UINTN  AllocationSize
  );

/**
  Allocates and zeros a buffer of type EfiBootServicesData.

  Allocates the number bytes specified by AllocationSize of type EfiBootServicesData, clears the
  buffer with zeros, and returns a pointer to the allocated buffer.  If AllocationSize is 0, then a
  valid buffer of 0 size is returned.  If there is not enough memory remaining to satisfy the
  request, then NULL is returned.

  @param  AllocationSize        The number of bytes to allocate and zero.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
BaseCryptAllocateZeroPool (
  IN UINTN  AllocationSize
  );

/**
  Frees a buffer that was previously allocated with one of the pool allocation functions.

  Frees the buffer specified by Buffer.  Buffer must have been allocated on a previous call to the
  pool allocation services of the Memory Allocation Library.  If it is not possible to free pool
  resources, then this function will perform no actions.

  If Buffer was not allocated with a pool allocation function, then ASSERT().

  @param  Buffer                Pointer to the buffer to free.

**/
VOID
EFIAPI
BaseCryptFreePool (
  IN VOID  *Buffer
  );

/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time          A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities  An optional pointer to a buffer to receive the real time clock
                        device's capabilities.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The time could not be retrieved due to hardware error.

**/
EFI_STATUS
EFIAPI
BaseCryptGetTime (
  OUT  EFI_TIME               *Time,
  OUT  EFI_TIME_CAPABILITIES  *Capabilities OPTIONAL
  );

/**
  Generates a 64-bit random number.

  @param[out] Rand     Buffer pointer to store the 64-bit random value.

  @retval TRUE         Random number generated successfully.
  @retval FALSE        Failed to generate the random number.

**/
BOOLEAN
EFIAPI
BaseCryptGetRandomNumber64 (
  OUT UINT64  *Rand
  );

#endif // ONE_CRYPTO_CRT_LIB_H_
