/** @file
  This sample application that is the simplest UEFI application possible.
  It simply prints "Hello Uefi!" to the UEFI Console Out device and stalls the CPU for 30 seconds.

  Copyright (C) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
// #include <Library/StandaloneMmDriverEntryPoint.h> # Intentionally not included
#include <Library/MmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Private/SharedCrtLibSupport.h>
#include <Library/OneCryptoDependencySupport.h>
#include <Library/BaseCryptLib.h>
#include <Private/OneCryptoLibrary.h>
#include <Protocol/OneCryptoProtocol.h>

//
// Function declarations from BaseCryptLib
//
CONST CHAR8 *
EFIAPI
GetOpenSslVersionText (
  VOID
  );

UINTN
EFIAPI
GetOpenSslVersionNumber (
  VOID
  );

// #if defined(_MSC_VER)
#define COMMON_EXPORT_API  __declspec(dllexport)

ONE_CRYPTO_MM_CONSTRUCTOR_PROTOCOL  *ProtocolInstance = NULL;

/**
  Initialize crypto functionality.
  
  This function populates the crypto protocol structure with function pointers
  from BaseCryptLib implementations.
  
  @param[in] Crypto  Pointer to crypto protocol structure to initialize.
**/
VOID
EFIAPI
CryptoInit (
  IN VOID *Crypto
  )
{
  ONE_CRYPTO_PROTOCOL *CryptoProtocol = (ONE_CRYPTO_PROTOCOL *)Crypto;
  
  if (CryptoProtocol == NULL) {
    DEBUG ((DEBUG_ERROR, "OneCryptoBin: CryptoInit called with NULL protocol\n"));
    return;
  }
  
  //
  // Set the Crypto Version
  //
  CryptoProtocol->Major    = VERSION_MAJOR;
  CryptoProtocol->Minor    = VERSION_MINOR;
  CryptoProtocol->Revision = VERSION_REVISION;

  //
  // Begin filling out the crypto protocol
  //

  //
  // Initialize HMAC-SHA256 function pointers
  //
  CryptoProtocol->HmacSha256New       = HmacSha256New;
  CryptoProtocol->HmacSha256Free      = HmacSha256Free;
  CryptoProtocol->HmacSha256SetKey    = HmacSha256SetKey;
  CryptoProtocol->HmacSha256Duplicate = HmacSha256Duplicate;
  CryptoProtocol->HmacSha256Update    = HmacSha256Update;
  CryptoProtocol->HmacSha256Final     = HmacSha256Final;
  CryptoProtocol->HmacSha256All       = HmacSha256All;

  //
  // Initialize HMAC-SHA384 function pointers
  //
  CryptoProtocol->HmacSha384New       = HmacSha384New;
  CryptoProtocol->HmacSha384Free      = HmacSha384Free;
  CryptoProtocol->HmacSha384SetKey    = HmacSha384SetKey;
  CryptoProtocol->HmacSha384Duplicate = HmacSha384Duplicate;
  CryptoProtocol->HmacSha384Update    = HmacSha384Update;
  CryptoProtocol->HmacSha384Final     = HmacSha384Final;
  CryptoProtocol->HmacSha384All       = HmacSha384All;

  //
  // Initialize the Big Num function pointers
  //
  CryptoProtocol->BigNumInit        = BigNumInit;
  CryptoProtocol->BigNumFromBin     = BigNumFromBin;
  CryptoProtocol->BigNumToBin       = BigNumToBin;
  CryptoProtocol->BigNumFree        = BigNumFree;
  CryptoProtocol->BigNumAdd         = BigNumAdd;
  CryptoProtocol->BigNumSub         = BigNumSub;
  CryptoProtocol->BigNumMod         = BigNumMod;
  CryptoProtocol->BigNumExpMod      = BigNumExpMod;
  CryptoProtocol->BigNumInverseMod  = BigNumInverseMod;
  CryptoProtocol->BigNumDiv         = BigNumDiv;
  CryptoProtocol->BigNumMulMod      = BigNumMulMod;
  CryptoProtocol->BigNumCmp         = BigNumCmp;
  CryptoProtocol->BigNumBits        = BigNumBits;
  CryptoProtocol->BigNumBytes       = BigNumBytes;
  CryptoProtocol->BigNumIsWord      = BigNumIsWord;
  CryptoProtocol->BigNumIsOdd       = BigNumIsOdd;
  CryptoProtocol->BigNumCopy        = BigNumCopy;
  CryptoProtocol->BigNumValueOne    = BigNumValueOne;
  CryptoProtocol->BigNumRShift      = BigNumRShift;
  CryptoProtocol->BigNumConstTime   = BigNumConstTime;
  CryptoProtocol->BigNumSqrMod      = BigNumSqrMod;
  CryptoProtocol->BigNumNewContext  = BigNumNewContext;
  CryptoProtocol->BigNumContextFree = BigNumContextFree;
  CryptoProtocol->BigNumSetUint     = BigNumSetUint;
  CryptoProtocol->BigNumAddMod      = BigNumAddMod;

  //
  // AES functions
  //
  CryptoProtocol->AeadAesGcmEncrypt = AeadAesGcmEncrypt;
  CryptoProtocol->AeadAesGcmDecrypt = AeadAesGcmDecrypt;
  CryptoProtocol->AesGetContextSize = AesGetContextSize;
  CryptoProtocol->AesInit           = AesInit;
  CryptoProtocol->AesCbcEncrypt     = AesCbcEncrypt;
  CryptoProtocol->AesCbcDecrypt     = AesCbcDecrypt;

 #ifdef ENABLE_MD5_DEPRECATED_INTERFACES
  CryptoProtocol->Md5GetContextSize = MD5GetContextSize;
  CryptoProtocol->Md5Init           = MD5Init;
  CryptoProtocol->Md5Update         = MD5Update;
  CryptoProtocol->Md5Final          = MD5Final;
  CryptoProtocol->Md5Duplicate      = MD5Duplicate;
  CryptoProtocol->Md5HashAll        = MD5HashAll;
 #else
  CryptoProtocol->Md5HashAll        = NULL;
  CryptoProtocol->Md5GetContextSize = NULL;
  CryptoProtocol->Md5Init           = NULL;
  CryptoProtocol->Md5Update         = NULL;
  CryptoProtocol->Md5Final          = NULL;
  CryptoProtocol->Md5Duplicate      = NULL;
 #endif // ENABLE_MD5_DEPRECATED_INTERFACES

  CryptoProtocol->Sha1GetContextSize = Sha1GetContextSize;
  CryptoProtocol->Sha1Init           = Sha1Init;
  CryptoProtocol->Sha1Update         = Sha1Update;
  CryptoProtocol->Sha1Final          = Sha1Final;
  CryptoProtocol->Sha1Duplicate      = Sha1Duplicate;
  CryptoProtocol->Sha1HashAll        = Sha1HashAll;

  CryptoProtocol->Sha256GetContextSize = Sha256GetContextSize;
  CryptoProtocol->Sha256Init           = Sha256Init;
  CryptoProtocol->Sha256Update         = Sha256Update;
  CryptoProtocol->Sha256Final          = Sha256Final;
  CryptoProtocol->Sha256Duplicate      = Sha256Duplicate;
  CryptoProtocol->Sha256HashAll        = Sha256HashAll;

  CryptoProtocol->Sha384GetContextSize = Sha384GetContextSize;
  CryptoProtocol->Sha384Init           = Sha384Init;
  CryptoProtocol->Sha384Update         = Sha384Update;
  CryptoProtocol->Sha384Final          = Sha384Final;
  CryptoProtocol->Sha384Duplicate      = Sha384Duplicate;
  CryptoProtocol->Sha384HashAll        = Sha384HashAll;

  CryptoProtocol->Sha512GetContextSize = Sha512GetContextSize;
  CryptoProtocol->Sha512Init           = Sha512Init;
  CryptoProtocol->Sha512Update         = Sha512Update;
  CryptoProtocol->Sha512Final          = Sha512Final;
  CryptoProtocol->Sha512Duplicate      = Sha512Duplicate;
  CryptoProtocol->Sha512HashAll        = Sha512HashAll;

  //
  // Set SM3 functions to NULL since they are disabled
  //
  CryptoProtocol->Sm3GetContextSize = NULL;
  CryptoProtocol->Sm3Init           = NULL;
  CryptoProtocol->Sm3Update         = NULL;
  CryptoProtocol->Sm3Final          = NULL;
  CryptoProtocol->Sm3Duplicate      = NULL;
  CryptoProtocol->Sm3HashAll        = NULL;

  // ========================================================================================================
  // Key Derivation Functions
  // ========================================================================================================
  CryptoProtocol->HkdfSha256Expand           = HkdfSha256Expand;
  CryptoProtocol->HkdfSha256Extract          = HkdfSha256Extract;
  CryptoProtocol->HkdfSha256ExtractAndExpand = HkdfSha256ExtractAndExpand;
  CryptoProtocol->HkdfSha384Expand           = HkdfSha384Expand;
  CryptoProtocol->HkdfSha384Extract          = HkdfSha384Extract;
  CryptoProtocol->HkdfSha384ExtractAndExpand = HkdfSha384ExtractAndExpand;

  // ========================================================================================================
  // Public Key Cryptography
  // ========================================================================================================
  CryptoProtocol->AuthenticodeVerify         = AuthenticodeVerify;
  CryptoProtocol->DhNew                      = DhNew;
  CryptoProtocol->DhFree                     = DhFree;
  CryptoProtocol->DhGenerateParameter        = DhGenerateParameter;
  CryptoProtocol->DhSetParameter             = DhSetParameter;
  CryptoProtocol->DhGenerateKey              = DhGenerateKey;
  CryptoProtocol->DhComputeKey               = DhComputeKey;
  CryptoProtocol->Pkcs5HashPassword          = Pkcs5HashPassword;
  CryptoProtocol->Pkcs1v2Encrypt             = Pkcs1v2Encrypt;
  CryptoProtocol->Pkcs1v2Decrypt             = Pkcs1v2Decrypt;
  CryptoProtocol->RsaOaepEncrypt             = RsaOaepEncrypt;
  CryptoProtocol->RsaOaepDecrypt             = RsaOaepDecrypt;
  CryptoProtocol->Pkcs7GetSigners            = Pkcs7GetSigners;
  CryptoProtocol->Pkcs7FreeSigners           = Pkcs7FreeSigners;
  CryptoProtocol->Pkcs7GetCertificatesList   = Pkcs7GetCertificatesList;
  CryptoProtocol->Pkcs7Verify                = Pkcs7Verify;
  CryptoProtocol->Pkcs7Sign                  = Pkcs7Sign;
  CryptoProtocol->Pkcs7Encrypt               = Pkcs7Encrypt;
  CryptoProtocol->VerifyEKUsInPkcs7Signature = VerifyEKUsInPkcs7Signature;
  CryptoProtocol->Pkcs7GetAttachedContent    = Pkcs7GetAttachedContent;

  // ========================================================================================================
  // Basic Elliptic Curve Primitives
  // ========================================================================================================
  CryptoProtocol->EcGroupInit                     = EcGroupInit;
  CryptoProtocol->EcGroupGetCurve                 = EcGroupGetCurve;
  CryptoProtocol->EcGroupGetOrder                 = EcGroupGetOrder;
  CryptoProtocol->EcGroupFree                     = EcGroupFree;
  CryptoProtocol->EcPointInit                     = EcPointInit;
  CryptoProtocol->EcPointDeInit                   = EcPointDeInit;
  CryptoProtocol->EcPointGetAffineCoordinates     = EcPointGetAffineCoordinates;
  CryptoProtocol->EcPointSetAffineCoordinates     = EcPointSetAffineCoordinates;
  CryptoProtocol->EcPointAdd                      = EcPointAdd;
  CryptoProtocol->EcPointMul                      = EcPointMul;
  CryptoProtocol->EcPointInvert                   = EcPointInvert;
  CryptoProtocol->EcPointIsOnCurve                = EcPointIsOnCurve;
  CryptoProtocol->EcPointIsAtInfinity             = EcPointIsAtInfinity;
  CryptoProtocol->EcPointEqual                    = EcPointEqual;
  CryptoProtocol->EcPointSetCompressedCoordinates = EcPointSetCompressedCoordinates;

  // ========================================================================================================
  // Elliptic Curve Diffie Hellman Primitives
  // ========================================================================================================

  CryptoProtocol->EcNewByNid             = EcNewByNid;
  CryptoProtocol->EcFree                 = EcFree;
  CryptoProtocol->EcGenerateKey          = EcGenerateKey;
  CryptoProtocol->EcGetPubKey            = EcGetPubKey;
  CryptoProtocol->EcDhComputeKey         = EcDhComputeKey;
  CryptoProtocol->EcGetPrivateKeyFromPem = EcGetPrivateKeyFromPem;
  CryptoProtocol->EcGetPublicKeyFromX509 = EcGetPublicKeyFromX509;
  CryptoProtocol->EcDsaSign              = EcDsaSign;
  CryptoProtocol->EcDsaVerify            = EcDsaVerify;

  // ========================================================================================================
  // RSA Primitives
  // ========================================================================================================
  CryptoProtocol->RsaNew                  = RsaNew;
  CryptoProtocol->RsaFree                 = RsaFree;
  CryptoProtocol->RsaSetKey               = RsaSetKey;
  CryptoProtocol->RsaGetKey               = RsaGetKey;
  CryptoProtocol->RsaGenerateKey          = RsaGenerateKey;
  CryptoProtocol->RsaCheckKey             = RsaCheckKey;
  CryptoProtocol->RsaPkcs1Sign            = RsaPkcs1Sign;
  CryptoProtocol->RsaPkcs1Verify          = RsaPkcs1Verify;
  CryptoProtocol->RsaPssSign              = RsaPssSign;
  CryptoProtocol->RsaPssVerify            = RsaPssVerify;
  CryptoProtocol->RsaGetPrivateKeyFromPem = RsaGetPrivateKeyFromPem;
  CryptoProtocol->RsaGetPublicKeyFromX509 = RsaGetPublicKeyFromX509;

  // ========================================================================================================
  // X509 Certificate Primitives
  // ========================================================================================================
  CryptoProtocol->X509GetSubjectName              = X509GetSubjectName;
  CryptoProtocol->X509GetCommonName               = X509GetCommonName;
  CryptoProtocol->X509GetOrganizationName         = X509GetOrganizationName;
  CryptoProtocol->X509VerifyCert                  = X509VerifyCert;
  CryptoProtocol->X509ConstructCertificate        = X509ConstructCertificate;
  CryptoProtocol->X509ConstructCertificateStackV  = X509ConstructCertificateStackV;
  CryptoProtocol->X509ConstructCertificateStack   = X509ConstructCertificateStack;
  CryptoProtocol->X509Free                        = X509Free;
  CryptoProtocol->X509StackFree                   = X509StackFree;
  CryptoProtocol->X509GetTBSCert                  = X509GetTBSCert;
  CryptoProtocol->X509GetVersion                  = X509GetVersion;
  CryptoProtocol->X509GetSerialNumber             = X509GetSerialNumber;
  CryptoProtocol->X509GetIssuerName               = X509GetIssuerName;
  CryptoProtocol->X509GetSignatureAlgorithm       = X509GetSignatureAlgorithm;
  CryptoProtocol->X509GetExtensionData            = X509GetExtensionData;
  CryptoProtocol->X509GetValidity                 = X509GetValidity;
  CryptoProtocol->X509FormatDateTime              = X509FormatDateTime;
  CryptoProtocol->X509GetKeyUsage                 = X509GetKeyUsage;
  CryptoProtocol->X509GetExtendedKeyUsage         = X509GetExtendedKeyUsage;
  CryptoProtocol->X509VerifyCertChain             = X509VerifyCertChain;
  CryptoProtocol->X509GetCertFromCertChain        = X509GetCertFromCertChain;
  CryptoProtocol->X509GetExtendedBasicConstraints = X509GetExtendedBasicConstraints;

  // ========================================================================================================
  // Random Number Generation
  // ========================================================================================================
  CryptoProtocol->RandomSeed  = RandomSeed;
  CryptoProtocol->RandomBytes = RandomBytes;

  // ========================================================================================================
  // TLS Primitives
  // ========================================================================================================
  CryptoProtocol->TlsInitialize              = TlsInitialize;
  CryptoProtocol->TlsCtxFree                 = TlsCtxFree;
  CryptoProtocol->TlsCtxNew                  = TlsCtxNew;
  CryptoProtocol->TlsFree                    = TlsFree;
  CryptoProtocol->TlsNew                     = TlsNew;
  CryptoProtocol->TlsInHandshake             = TlsInHandshake;
  CryptoProtocol->TlsDoHandshake             = TlsDoHandshake;
  CryptoProtocol->TlsHandleAlert             = TlsHandleAlert;
  CryptoProtocol->TlsCloseNotify             = TlsCloseNotify;
  CryptoProtocol->TlsCtrlTrafficOut          = TlsCtrlTrafficOut;
  CryptoProtocol->TlsCtrlTrafficIn           = TlsCtrlTrafficIn;
  CryptoProtocol->TlsRead                    = TlsRead;
  CryptoProtocol->TlsWrite                   = TlsWrite;
  CryptoProtocol->TlsShutdown                = TlsShutdown;
  CryptoProtocol->TlsSetVersion              = TlsSetVersion;
  CryptoProtocol->TlsSetConnectionEnd        = TlsSetConnectionEnd;
  CryptoProtocol->TlsSetCipherList           = TlsSetCipherList;
  CryptoProtocol->TlsSetCompressionMethod    = TlsSetCompressionMethod;
  CryptoProtocol->TlsSetVerify               = TlsSetVerify;
  CryptoProtocol->TlsSetVerifyHost           = TlsSetVerifyHost;
  CryptoProtocol->TlsSetSessionId            = TlsSetSessionId;
  CryptoProtocol->TlsSetCaCertificate        = TlsSetCaCertificate;
  CryptoProtocol->TlsSetHostPublicCert       = TlsSetHostPublicCert;
  CryptoProtocol->TlsSetHostPrivateKeyEx     = TlsSetHostPrivateKeyEx;
  CryptoProtocol->TlsSetHostPrivateKey       = TlsSetHostPrivateKey;
  CryptoProtocol->TlsSetCertRevocationList   = TlsSetCertRevocationList;
  CryptoProtocol->TlsSetSignatureAlgoList    = TlsSetSignatureAlgoList;
  CryptoProtocol->TlsSetEcCurve              = TlsSetEcCurve;
  CryptoProtocol->TlsGetVersion              = TlsGetVersion;
  CryptoProtocol->TlsGetConnectionEnd        = TlsGetConnectionEnd;
  CryptoProtocol->TlsGetCurrentCipher        = TlsGetCurrentCipher;
  CryptoProtocol->TlsGetCurrentCompressionId = TlsGetCurrentCompressionId;
  CryptoProtocol->TlsGetVerify               = TlsGetVerify;
  CryptoProtocol->TlsGetSessionId            = TlsGetSessionId;
  CryptoProtocol->TlsGetClientRandom         = TlsGetClientRandom;
  CryptoProtocol->TlsGetServerRandom         = TlsGetServerRandom;
  CryptoProtocol->TlsGetKeyMaterial          = TlsGetKeyMaterial;
  CryptoProtocol->TlsGetCaCertificate        = TlsGetCaCertificate;
  CryptoProtocol->TlsGetHostPublicCert       = TlsGetHostPublicCert;
  CryptoProtocol->TlsGetHostPrivateKey       = TlsGetHostPrivateKey;
  CryptoProtocol->TlsGetCertRevocationList   = TlsGetCertRevocationList;
  CryptoProtocol->TlsGetExportKey            = TlsGetExportKey;

  // ========================================================================================================
  // Timestamp Primitives
  // ========================================================================================================

  CryptoProtocol->ImageTimestampVerify = ImageTimestampVerify;

  DEBUG ((DEBUG_INFO, "OneCryptoBin: CryptoInit completed - comprehensive protocol populated\n"));
}

COMMON_EXPORT_API
EFI_STATUS
EFIAPI
Constructor (
  IN SHARED_DEPENDENCIES  *Depends,
  OUT VOID                **Crypto
  )
{
  //
  // Map the provided depencencies to our global instance
  //
  gSharedDepends = Depends;

  DEBUG ((DEBUG_ERROR, "OneCryptoBin: Constructor entry called\n"));

  //
  // Allocate the required space for our ONE_CRYPTO_PROTOCOL
  // We do not care that we might be publishing more functions than available via the library instance
  // the library should be responsible for ensuing that the crypto functionality is acceptable before
  // attempting to use it.
  // 
  *Crypto = AllocateZeroPool (sizeof (ONE_CRYPTO_PROTOCOL));
  if (*Crypto == NULL) {
    DEBUG ((DEBUG_ERROR, "OneCryptoBin: Failed to allocate memory for Crypto protocol\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_ERROR, "OpenSSL Version: %a (0x%lx)\n", GetOpenSslVersionText(), (UINT64)GetOpenSslVersionNumber()));

  //
  // Initialize the Crypto Protocol
  //
  CryptoInit (*Crypto);

  return EFI_SUCCESS;
}

/**
  MM Entry Point for the Shared Crypto MM Driver.

  @param[in] ImageHandle      The firmware allocated handle for the EFI image.
  @param[in] MmSystemTable    A pointer to the MM System Table.

  @retval EFI_SUCCESS         The entry point executed successfully.
  @retval EFI_OUT_OF_RESOURCES Failed to allocate memory.
  @retval other               Error returned by protocol installation.
**/
EFI_STATUS
EFIAPI
MmEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_MM_SYSTEM_TABLE  *MmSystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle = NULL;

  if (MmSystemTable == NULL) {
    DEBUG ((DEBUG_ERROR, "OneCryptoBin: MmSystemTable is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = MmSystemTable->MmAllocatePool (
                            EfiRuntimeServicesData,
                            sizeof (ONE_CRYPTO_MM_CONSTRUCTOR_PROTOCOL),
                            (VOID **)&ProtocolInstance
                            );

  if (EFI_ERROR (Status) || (ProtocolInstance == NULL)) {
    DEBUG ((DEBUG_ERROR, "OneCryptoBin: Failed to allocate memory for constructor protocol: %r\n", Status));
    return EFI_OUT_OF_RESOURCES;
  }

  ProtocolInstance->Signature   = ONE_CRYPTO_MM_CONSTRUCTOR_PROTOCOL_SIGNATURE;
  ProtocolInstance->Version     = 1;
  ProtocolInstance->Constructor = Constructor;

  Status = MmSystemTable->MmInstallProtocolInterface (
                            &Handle,
                            &gOneCryptoPrivateProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            ProtocolInstance
                            );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "OneCryptoBin: Failed to install protocol: %r\n", Status));
    MmSystemTable->MmFreePool (ProtocolInstance);
    ProtocolInstance = NULL;
    return Status;
  }

  DEBUG ((DEBUG_INFO, "OneCryptoBin: Protocol installed successfully\n"));

  return EFI_SUCCESS;
}
