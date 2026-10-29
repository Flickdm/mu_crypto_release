/** @file
  Exercise CMS and PQC using the production UEFI OpenSSL provider.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseCryptLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UnitTestLib.h>
#include <openssl/cms.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pkcs7.h>
#include <openssl/x509.h>
#include "PqcFixtures.h"

VOID EFIAPI
ProcessLibraryConstructorList (
  VOID
  );

STATIC CONST UINT8  mMessage[]         = "UEFI CMS provider regression";
STATIC UINTN        mCmsPaddingSizes[] = { 50000, 70000 };

STATIC
UNIT_TEST_STATUS
EFIAPI
CmsShortInput (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  CMS_FIXTURE  *Fixture;
  UINT8        *Input;
  UINTN        Size;
  BOOLEAN      Rejected;

  Fixture = &mCmsFixtures[0];
  for (Size = 0; Size < 19; Size++) {
    Input = AllocateCopyPool ((Size == 0) ? 1 : Size, Fixture->Cms);
    UT_ASSERT_NOT_NULL (Input);
    Rejected = (BOOLEAN) !Pkcs7Verify (
                            Input,
                            Size,
                            Fixture->Certificate,
                            Fixture->CertificateSize,
                            mMessage,
                            sizeof (mMessage)
                            );
    FreePool (Input);
    UT_ASSERT_TRUE (Rejected);
  }

  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
CmsSmallLengthEncoding (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  PKCS7    *Pkcs7;
  UINT8    Payload[256] = { 0 };
  UINT8    *Der;
  VOID     *Content;
  UINTN    ContentSize;
  UINTN    Size;
  int      DerSize;
  BOOLEAN  Passed;

  Pkcs7  = PKCS7_new ();
  Der    = NULL;
  Passed = FALSE;
  if ((Pkcs7 == NULL) || (PKCS7_set_type (Pkcs7, NID_pkcs7_signed) != 1) ||
      (PKCS7_content_new (Pkcs7, NID_pkcs7_data) != 1))
  {
    goto Exit;
  }

  for (Size = 64; Size <= sizeof (Payload); Size *= 2) {
    Passed = FALSE;
    if (ASN1_OCTET_STRING_set (Pkcs7->d.sign->contents->d.data, Payload, (int)Size) != 1) {
      goto Exit;
    }

    DerSize = i2d_PKCS7 (Pkcs7, &Der);
    if (DerSize <= 0) {
      goto Exit;
    }

    Content     = NULL;
    ContentSize = 0;
    Passed      = Pkcs7GetAttachedContent (Der, (UINTN)DerSize, &Content, &ContentSize);
    Passed      = (BOOLEAN)(Passed && (ContentSize == Size) && (Content != NULL) &&
                            (CompareMem (Content, Payload, Size) == 0));
    if (Content != NULL) {
      FreePool (Content);
    }

    OPENSSL_free (Der);
    Der = NULL;
    if (!Passed) {
      goto Exit;
    }
  }

Exit:
  PKCS7_free (Pkcs7);
  OPENSSL_free (Der);
  UT_ASSERT_TRUE (Passed);
  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
CmsLengthEncoding (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  CMS_FIXTURE      *Fixture;
  CMS_ContentInfo  *Cms;
  CMS_SignerInfo   *Signer;
  PKCS7            *Pkcs7;
  CONST UINT8      *Cursor;
  UINT8            *Padding;
  UINT8            *Der;
  UINT8            *RawDer;
  UINTN            PaddingSize;
  int              DerSize;
  int              RawSize;
  BOOLEAN          Passed;

  Fixture     = &mCmsFixtures[0];
  PaddingSize = *(UINTN *)Context;
  Cursor      = Fixture->Cms;
  Cms         = d2i_CMS_ContentInfo (NULL, &Cursor, (long)Fixture->CmsSize);
  Pkcs7       = NULL;
  Padding     = OPENSSL_zalloc (PaddingSize);
  Der         = NULL;
  RawDer      = NULL;
  Passed      = FALSE;
  if ((Cms == NULL) || (Padding == NULL)) {
    goto Exit;
  }

  Signer = sk_CMS_SignerInfo_value (CMS_get0_SignerInfos (Cms), 0);
  if ((Signer == NULL) ||
      (CMS_unsigned_add1_attr_by_NID (Signer, NID_pkcs9_unstructuredName, V_ASN1_OCTET_STRING, Padding, (int)PaddingSize) != 1))
  {
    goto Exit;
  }

  DerSize = i2d_CMS_ContentInfo (Cms, &Der);
  if ((DerSize <= 0) || ((DerSize > MAX_UINT16) != (PaddingSize > MAX_UINT16)) ||
      !Pkcs7Verify (Der, (UINTN)DerSize, Fixture->Certificate, Fixture->CertificateSize, mMessage, sizeof (mMessage)))
  {
    goto Exit;
  }

  Cursor = Der;
  Pkcs7  = d2i_PKCS7 (NULL, &Cursor, DerSize);
  if (Pkcs7 == NULL) {
    goto Exit;
  }

  RawSize = i2d_PKCS7_SIGNED (Pkcs7->d.sign, &RawDer);
  Passed  = (BOOLEAN)((RawSize > 0) &&
                      Pkcs7Verify (RawDer, (UINTN)RawSize, Fixture->Certificate, Fixture->CertificateSize, mMessage, sizeof (mMessage)));

Exit:
  PKCS7_free (Pkcs7);
  CMS_ContentInfo_free (Cms);
  OPENSSL_free (Padding);
  OPENSSL_free (Der);
  OPENSSL_free (RawDer);
  UT_ASSERT_TRUE (Passed);
  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
AuthenticodeContentType (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  STATIC CONST UINT8  BooleanContent[] = { 0x01, 0x01, 0xFF };
  PKCS7               *Pkcs7;
  PKCS7               *Content;
  CONST UINT8         *Cursor;
  UINT8               *Der;
  UINT8               Hash[32] = { 0 };
  int                 DerSize;
  BOOLEAN             Passed;

  Pkcs7  = PKCS7_new ();
  Der    = NULL;
  Passed = FALSE;
  if ((Pkcs7 == NULL) || (PKCS7_set_type (Pkcs7, NID_pkcs7_signed) != 1)) {
    goto Exit;
  }

  Content          = Pkcs7->d.sign->contents;
  Cursor           = BooleanContent;
  Content->d.other = d2i_ASN1_TYPE (NULL, &Cursor, sizeof (BooleanContent));
  ASN1_OBJECT_free (Content->type);
  Content->type = OBJ_txt2obj ("1.3.6.1.4.1.311.2.1.4", 1);
  if ((Content->d.other == NULL) || (Content->type == NULL)) {
    goto Exit;
  }

  DerSize = i2d_PKCS7 (Pkcs7, &Der);
  Passed  = (BOOLEAN)((DerSize > 0) &&
                      !AuthenticodeVerify (Der, (UINTN)DerSize, mUntrustedCertificate, sizeof (mUntrustedCertificate), Hash, sizeof (Hash)));

Exit:
  PKCS7_free (Pkcs7);
  OPENSSL_free (Der);
  UT_ASSERT_TRUE (Passed);
  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
CmsRoundTrip (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  CMS_FIXTURE  *Fixture;
  UINT8        *CmsDer;
  UINT8        Modified[sizeof (mMessage)];
  BOOLEAN      Passed;

  Fixture = Context;
  CmsDer  = AllocateCopyPool (Fixture->CmsSize, Fixture->Cms);
  UT_ASSERT_NOT_NULL (CmsDer);
  Passed = FALSE;
  if (!Pkcs7Verify (CmsDer, Fixture->CmsSize, Fixture->Certificate, Fixture->CertificateSize, mMessage, sizeof (mMessage))) {
    goto Exit;
  }

  CopyMem (Modified, mMessage, sizeof (Modified));
  Modified[0] ^= 1;
  if (Pkcs7Verify (CmsDer, Fixture->CmsSize, Fixture->Certificate, Fixture->CertificateSize, Modified, sizeof (Modified)) ||
      Pkcs7Verify (CmsDer, Fixture->CmsSize - 1, Fixture->Certificate, Fixture->CertificateSize, mMessage, sizeof (mMessage)))
  {
    goto Exit;
  }

  if (Pkcs7Verify (CmsDer, Fixture->CmsSize, mUntrustedCertificate, sizeof (mUntrustedCertificate), mMessage, sizeof (mMessage))) {
    goto Exit;
  }

  CmsDer[Fixture->CmsSize - 1] ^= 1;
  Passed                        = (BOOLEAN) !Pkcs7Verify (CmsDer, Fixture->CmsSize, Fixture->Certificate, Fixture->CertificateSize, mMessage, sizeof (mMessage));

Exit:
  if (!Passed) {
    UT_LOG_ERROR ("CMS fixture failed: 0x%Lx\n", (UINT64)ERR_peek_last_error ());
  }

  FreePool (CmsDer);
  UT_ASSERT_TRUE (Passed);
  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
KemRoundTrip (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EVP_PKEY      *Key;
  EVP_PKEY_CTX  *Encap;
  EVP_PKEY_CTX  *Decap;
  UINT8         Ciphertext[2048];
  UINT8         Secret[32];
  UINT8         Recovered[32];
  size_t        CiphertextSize;
  size_t        SecretSize;
  size_t        RecoveredSize;
  BOOLEAN       Passed;

  Key            = EVP_PKEY_Q_keygen (NULL, NULL, (CONST CHAR8 *)Context);
  Encap          = NULL;
  Decap          = NULL;
  CiphertextSize = sizeof (Ciphertext);
  SecretSize     = sizeof (Secret);
  RecoveredSize  = sizeof (Recovered);
  Passed         = FALSE;
  if (Key == NULL) {
    goto Exit;
  }

  Encap = EVP_PKEY_CTX_new_from_pkey (NULL, Key, NULL);
  Decap = EVP_PKEY_CTX_new_from_pkey (NULL, Key, NULL);
  if ((Encap == NULL) || (Decap == NULL) ||
      (EVP_PKEY_encapsulate_init (Encap, NULL) != 1) ||
      (EVP_PKEY_decapsulate_init (Decap, NULL) != 1) ||
      (EVP_PKEY_encapsulate (Encap, Ciphertext, &CiphertextSize, Secret, &SecretSize) != 1) ||
      (EVP_PKEY_decapsulate (Decap, Recovered, &RecoveredSize, Ciphertext, CiphertextSize) != 1) ||
      (SecretSize != sizeof (Secret)) || (RecoveredSize != SecretSize) ||
      (CompareMem (Secret, Recovered, SecretSize) != 0))
  {
    goto Exit;
  }

  Ciphertext[0] ^= 1;
  RecoveredSize  = sizeof (Recovered);
  Passed         = (BOOLEAN)(
                             (EVP_PKEY_decapsulate (Decap, Recovered, &RecoveredSize, Ciphertext, CiphertextSize) == 1) &&
                             (RecoveredSize == SecretSize) &&
                             (CompareMem (Secret, Recovered, SecretSize) != 0)
                             );

Exit:
  if (!Passed) {
    UT_LOG_ERROR ("KEM round trip %a failed: 0x%Lx\n", (CONST CHAR8 *)Context, (UINT64)ERR_peek_last_error ());
  }

  EVP_PKEY_CTX_free (Decap);
  EVP_PKEY_CTX_free (Encap);
  EVP_PKEY_free (Key);
  UT_ASSERT_TRUE (Passed);
  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
SlhCmsRejected (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  CMS_FIXTURE  *Fixture;

  Fixture = Context;
  UT_ASSERT_FALSE (Pkcs7Verify (Fixture->Cms, Fixture->CmsSize, Fixture->Certificate, Fixture->CertificateSize, mMessage, sizeof (mMessage)));
  return UNIT_TEST_PASSED;
}

STATIC
UNIT_TEST_STATUS
EFIAPI
SlhDisabled (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  STATIC CONST CHAR8  *Names[] = {
    "SLH-DSA-SHA2-128s",  "SLH-DSA-SHA2-128f",
    "SLH-DSA-SHA2-192s",  "SLH-DSA-SHA2-192f",
    "SLH-DSA-SHA2-256s",  "SLH-DSA-SHA2-256f",
    "SLH-DSA-SHAKE-128s", "SLH-DSA-SHAKE-128f",
    "SLH-DSA-SHAKE-192s", "SLH-DSA-SHAKE-192f",
    "SLH-DSA-SHAKE-256s", "SLH-DSA-SHAKE-256f"
  };
  UINTN               Index;
  EVP_SIGNATURE       *Signature;
  EVP_KEYMGMT         *KeyManagement;
  BOOLEAN             Unavailable;

  for (Index = 0; Index < ARRAY_SIZE (Names); Index++) {
    Signature     = EVP_SIGNATURE_fetch (NULL, Names[Index], NULL);
    KeyManagement = EVP_KEYMGMT_fetch (NULL, Names[Index], NULL);
    Unavailable   = (BOOLEAN)((Signature == NULL) && (KeyManagement == NULL));
    EVP_SIGNATURE_free (Signature);
    EVP_KEYMGMT_free (KeyManagement);
    UT_ASSERT_TRUE (Unavailable);
  }

  return UNIT_TEST_PASSED;
}

int
main (
  int   Argc,
  char  *Argv[]
  )
{
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      Suite;
  EFI_STATUS                  Status;

  ProcessLibraryConstructorList ();
  if (!RandomSeed (NULL, 0)) {
    return 1;
  }

  Framework = NULL;
  Status    = InitUnitTestFramework (&Framework, "UEFI CMS and PQC", "PQC", "1.0");
  if (EFI_ERROR (Status)) {
    return 1;
  }

  Status = CreateUnitTestSuite (&Suite, Framework, "UEFI provider operations", "PQC", NULL, NULL);
  if (!EFI_ERROR (Status)) {
    AddTestCase (Suite, "Empty and short CMS inputs are rejected", "CMS-Short", CmsShortInput, NULL, NULL, NULL);
    AddTestCase (Suite, "Short-form, one-byte, and two-byte CMS lengths parse", "CMS-SmallLengths", CmsSmallLengthEncoding, NULL, NULL, NULL);
    AddTestCase (Suite, "Wrapped and bare CMS below 64 KiB verify", "CMS-Length16", CmsLengthEncoding, NULL, NULL, &mCmsPaddingSizes[0]);
    AddTestCase (Suite, "Wrapped and bare CMS above 64 KiB verify", "CMS-Length24", CmsLengthEncoding, NULL, NULL, &mCmsPaddingSizes[1]);
    AddTestCase (Suite, "Authenticode rejects non-SEQUENCE content", "Authenticode-Type", AuthenticodeContentType, NULL, NULL, NULL);
    AddTestCase (Suite, "ML-DSA-44 CMS and rejection cases", "ML-DSA-44", CmsRoundTrip, NULL, NULL, &mCmsFixtures[0]);
    AddTestCase (Suite, "ML-DSA-65 CMS and rejection cases", "ML-DSA-65", CmsRoundTrip, NULL, NULL, &mCmsFixtures[1]);
    AddTestCase (Suite, "ML-DSA-87 CMS and rejection cases", "ML-DSA-87", CmsRoundTrip, NULL, NULL, &mCmsFixtures[2]);
    AddTestCase (Suite, "Unsupported SLH-DSA SHA2 CMS is rejected", "SLH-SHA2", SlhCmsRejected, NULL, NULL, &mCmsFixtures[3]);
    AddTestCase (Suite, "Unsupported SLH-DSA SHAKE CMS is rejected", "SLH-SHAKE", SlhCmsRejected, NULL, NULL, &mCmsFixtures[4]);
    AddTestCase (Suite, "ML-KEM-512 encapsulation and implicit rejection", "ML-KEM-512", KemRoundTrip, NULL, NULL, "ML-KEM-512");
    AddTestCase (Suite, "ML-KEM-768 encapsulation and implicit rejection", "ML-KEM-768", KemRoundTrip, NULL, NULL, "ML-KEM-768");
    AddTestCase (Suite, "ML-KEM-1024 encapsulation and implicit rejection", "ML-KEM-1024", KemRoundTrip, NULL, NULL, "ML-KEM-1024");
    AddTestCase (Suite, "All twelve SLH-DSA variants are unavailable", "SLH-Disabled", SlhDisabled, NULL, NULL, NULL);
    Status = RunAllTestSuites (Framework);
  }

  FreeUnitTestFramework (Framework);
  OPENSSL_cleanup ();
  return EFI_ERROR (Status) ? 1 : 0;
}
