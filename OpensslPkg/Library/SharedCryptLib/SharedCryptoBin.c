/** @file
  This sample application that is the simplest UEFI application possible.
  It simply prints "Hello Uefi!" to the UEFI Console Out device and stalls the CPU for 30 seconds.

  Copyright (C) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include "Shared/SharedOpenssl.h"
#include <SharedCrtLibSupport.h>

// #if defined(_MSC_VER)
#define COMMON_EXPORT_API  __declspec(dllexport)
// #else
// #define COMMON_EXPORT_API
// #endif


COMMON_EXPORT_API
EFI_STATUS
EFIAPI
Constructor (
  IN SHARED_DEPENDENCIES *Depends,
  OUT VOID  *RequestedCrypto
  )
{
  //
  // Map the provided depencencies to our global instance
  //
  gSharedDepends = Depends;

  // TODO DEBUG_ERROR = DEBUG_INFO?
  DEBUG((DEBUG_ERROR, "SharedCryptoBin: Constructor entry called\n"));

  //
  // Build the Crypto
  //
  CryptoInit (RequestedCrypto);

  return EFI_SUCCESS;
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle        The firmware allocated handle for the EFI image.
  @param[in] SystemTable        A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The entry point is executed successfully.
  @retval EFI_INVALID_PARAMETER SystemTable provided was not valid.
  @retval other                 Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  SHARED_CRYPTO_PROTOCOL  Crypto;

  CryptoInit (&Crypto);

  return EFI_SUCCESS;
}
