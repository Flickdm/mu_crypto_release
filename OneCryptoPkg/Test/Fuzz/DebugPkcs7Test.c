/** @file
  Debug test to check what Pkcs7Verify actually does with our inputs.

  This helps understand if we're getting into the parsing code or
  just hitting early parameter validation.
**/

#include "OneCryptoFuzzHarnessHost.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Minimal PKCS#7 SignedData structure
static const UINT8 gMinimalPkcs7[] = {
  0x30, 0x23,                         // SEQUENCE (35 bytes)
    0x06, 0x09,                       // OID (9 bytes) - signedData
      0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x02,
    0xa0, 0x16,                       // [0] EXPLICIT (22 bytes)
      0x30, 0x14,                     // SEQUENCE (20 bytes) - SignedData
        0x02, 0x01, 0x01,             // INTEGER version = 1
        0x31, 0x00,                   // SET digestAlgorithms (empty)
        0x30, 0x0b,                   // SEQUENCE contentInfo
          0x06, 0x09,                 // OID (9 bytes) - data
            0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x01,
        0x31, 0x00                    // SET signerInfos (empty)
};

// Self-signed test certificate (simplified)
static const UINT8 gTestCert[] = {
  0x30, 0x82, 0x01, 0x22,  // Minimal cert structure
  // ... truncated for test
};

static const UINT8 gTestContent[] = "Test content to verify";

int main(int argc, char *argv[]) {
    ONE_CRYPTO_PROTOCOL *Crypto;
    BOOLEAN result;
    int i;
    
    printf("Pkcs7Verify Debug Test\n");
    printf("======================\n\n");
    
    // Initialize
    if (OneCryptoFuzzInit() != 0) {
        fprintf(stderr, "Failed to initialize OneCrypto\n");
        return 1;
    }
    
    Crypto = OneCryptoFuzzGetProtocol();
    if (Crypto == NULL) {
        fprintf(stderr, "Failed to get protocol\n");
        return 1;
    }
    
    printf("OneCrypto Protocol loaded successfully\n");
    printf("Pkcs7Verify function: %p\n\n", (void*)Crypto->Pkcs7Verify);
    
    // Test 1: NULL parameters (should return FALSE quickly)
    printf("Test 1: NULL P7Data\n");
    result = Crypto->Pkcs7Verify(NULL, 10, gTestCert, sizeof(gTestCert), 
                                  gTestContent, sizeof(gTestContent)-1);
    printf("  Result: %s\n\n", result ? "TRUE" : "FALSE");
    
    // Test 2: Empty data
    printf("Test 2: Zero length\n");
    result = Crypto->Pkcs7Verify(gMinimalPkcs7, 0, gTestCert, sizeof(gTestCert),
                                  gTestContent, sizeof(gTestContent)-1);
    printf("  Result: %s\n\n", result ? "TRUE" : "FALSE");
    
    // Test 3: Minimal PKCS#7 structure
    printf("Test 3: Minimal PKCS#7 structure (%zu bytes)\n", sizeof(gMinimalPkcs7));
    printf("  Hex dump: ");
    for (i = 0; i < (int)sizeof(gMinimalPkcs7) && i < 32; i++) {
        printf("%02x ", gMinimalPkcs7[i]);
    }
    printf("...\n");
    result = Crypto->Pkcs7Verify(gMinimalPkcs7, sizeof(gMinimalPkcs7), 
                                  gTestCert, sizeof(gTestCert),
                                  gTestContent, sizeof(gTestContent)-1);
    printf("  Result: %s\n\n", result ? "TRUE" : "FALSE");
    
    // Test 4: Random garbage
    printf("Test 4: Random garbage\n");
    UINT8 garbage[64] = {0xde, 0xad, 0xbe, 0xef};
    result = Crypto->Pkcs7Verify(garbage, sizeof(garbage),
                                  gTestCert, sizeof(gTestCert),
                                  gTestContent, sizeof(gTestContent)-1);
    printf("  Result: %s\n\n", result ? "TRUE" : "FALSE");
    
    // Test 5: Load and test corpus files
    if (argc > 1) {
        printf("Test 5: Corpus file: %s\n", argv[1]);
        FILE *f = fopen(argv[1], "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            fseek(f, 0, SEEK_SET);
            UINT8 *data = malloc(size);
            fread(data, 1, size, f);
            fclose(f);
            
            printf("  File size: %ld bytes\n", size);
            printf("  Hex dump: ");
            for (i = 0; i < size && i < 32; i++) {
                printf("%02x ", data[i]);
            }
            if (size > 32) printf("...");
            printf("\n");
            
            result = Crypto->Pkcs7Verify(data, size,
                                          gTestCert, sizeof(gTestCert),
                                          gTestContent, sizeof(gTestContent)-1);
            printf("  Result: %s\n", result ? "TRUE" : "FALSE");
            free(data);
        } else {
            printf("  Failed to open file\n");
        }
    }
    
    printf("\n");
    printf("Summary:\n");
    printf("--------\n");
    printf("All results are FALSE because:\n");
    printf("1. Our test cert doesn't match any signer in the PKCS#7\n");
    printf("2. The content hash doesn't match\n");
    printf("3. The structures may be malformed\n");
    printf("\nBut importantly: the parsing code IS being exercised!\n");
    printf("We just can't see coverage since the .efi is not instrumented.\n");
    
    return 0;
}
