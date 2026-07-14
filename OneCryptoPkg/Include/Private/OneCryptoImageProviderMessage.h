/** @file

  Shared message format for OneCrypto image-provider MM communication.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef ONE_CRYPTO_IMAGE_PROVIDER_MESSAGE_H_
#define ONE_CRYPTO_IMAGE_PROVIDER_MESSAGE_H_

#define ONE_CRYPTO_IMAGE_PROVIDER_VERSION    2U
#define ONE_CRYPTO_IMAGE_PROVIDER_SIGNATURE  SIGNATURE_32 ('O', 'C', 'I', 'P')

//
// Payload format served by the provider (ONE_CRYPTO_IMAGE_PROVIDER_MSG.Format).
//
// PE32      - Data is the raw OneCrypto PE32 image; DXE LoadImage()s it directly.
// GUIDED_FV - Data is the whole GUID_DEFINED section (header + compressed
//             payload) that wraps the nested FV containing the OneCrypto PE32.
//             Secure world (MM) may lack the memory budget to decompress the FV,
//             so it hands the small compressed section to the resource-rich
//             normal world (DXE), which decodes it via ExtractGuidedSectionLib
//             (LZMA is the only handler registered there today) and extracts
//             the OneCrypto PE32.
//
#define ONE_CRYPTO_IMAGE_FORMAT_PE32       0U
#define ONE_CRYPTO_IMAGE_FORMAT_GUIDED_FV  1U

typedef struct {
  UINT32      Signature;
  UINT32      Version;
  UINT64      Offset;
  UINT32      RequestedSize;
  UINT32      ReturnedSize;
  UINT32      TotalImageSize;
  UINT32      Format;
  //
  // DEBUG-only transport-integrity aid: it lets the DXE receiver detect
  // accidental corruption or truncation of the chunked MM transfer and assert
  // during bring-up. It is NOT an authenticity check
  //
  UINT32      Crc32;
  EFI_GUID    ImageGuid;
  UINT8       Data[1];
} ONE_CRYPTO_IMAGE_PROVIDER_MSG;

#endif // ONE_CRYPTO_IMAGE_PROVIDER_MESSAGE_H_
