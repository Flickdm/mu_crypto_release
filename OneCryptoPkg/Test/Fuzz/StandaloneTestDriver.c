/** @file
  StandaloneTestDriver.c

  Simple standalone test driver for OneCrypto fuzzing harness.
  This can be used to test the harness without libFuzzer installed.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OneCryptoFuzzHarnessHost.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// External fuzzer entry point
//
extern int LLVMFuzzerTestOneInput (const uint8_t *Data, size_t Size);
extern int LLVMFuzzerInitialize (int *argc, char ***argv);

/**
  Read a file into memory.

  @param[in]   Path  Path to the file.
  @param[out]  Size  Size of the file.

  @return Pointer to file contents, or NULL on failure. Caller must free.
**/
static
uint8_t *
ReadFile (
  const char  *Path,
  size_t      *Size
  )
{
  FILE     *File;
  uint8_t  *Data;
  size_t   FileSize;

  File = fopen (Path, "rb");
  if (File == NULL) {
    return NULL;
  }

  fseek (File, 0, SEEK_END);
  FileSize = ftell (File);
  fseek (File, 0, SEEK_SET);

  Data = (uint8_t *)malloc (FileSize);
  if (Data == NULL) {
    fclose (File);
    return NULL;
  }

  if (fread (Data, 1, FileSize, File) != FileSize) {
    free (Data);
    fclose (File);
    return NULL;
  }

  fclose (File);
  *Size = FileSize;
  return Data;
}

/**
  Main entry point for standalone testing.

  Usage:
    ./TestDriver                    # Run with hardcoded test vectors
    ./TestDriver <file1> <file2>    # Run with input files
    ./TestDriver -                  # Read from stdin

  @param[in]  argc  Argument count.
  @param[in]  argv  Argument vector.

  @return 0 on success, non-zero on failure.
**/
int
main (
  int   argc,
  char  *argv[]
  )
{
  int      Result;
  int      i;
  uint8_t  *Data;
  size_t   Size;

  printf ("OneCrypto Standalone Test Driver\n");
  printf ("================================\n\n");

  //
  // Initialize the fuzzer
  //
  Result = LLVMFuzzerInitialize (&argc, &argv);
  if (Result != 0) {
    fprintf (stderr, "Failed to initialize fuzzer: %d\n", Result);
    return 1;
  }

  printf ("Harness initialized successfully.\n\n");

  if (argc > 1) {
    //
    // Process input files
    //
    for (i = 1; i < argc; i++) {
      if (strcmp (argv[i], "-") == 0) {
        //
        // Read from stdin
        //
        uint8_t  Buffer[65536];
        Size = fread (Buffer, 1, sizeof (Buffer), stdin);
        printf ("Testing stdin input (%zu bytes)...\n", Size);
        LLVMFuzzerTestOneInput (Buffer, Size);
      } else {
        //
        // Read from file
        //
        Data = ReadFile (argv[i], &Size);
        if (Data == NULL) {
          fprintf (stderr, "Failed to read file: %s\n", argv[i]);
          continue;
        }

        printf ("Testing %s (%zu bytes)...\n", argv[i], Size);
        LLVMFuzzerTestOneInput (Data, Size);
        free (Data);
      }
    }
  } else {
    //
    // Run with hardcoded test vectors
    //
    printf ("Running hardcoded test vectors...\n\n");

    // Test 1: Empty input
    printf ("Test 1: Empty input\n");
    LLVMFuzzerTestOneInput ((const uint8_t *)"", 0);
    printf ("  PASSED\n");

    // Test 2: Single byte
    printf ("Test 2: Single byte\n");
    LLVMFuzzerTestOneInput ((const uint8_t *)"A", 1);
    printf ("  PASSED\n");

    // Test 3: "Hello, World!"
    printf ("Test 3: Hello, World!\n");
    LLVMFuzzerTestOneInput ((const uint8_t *)"Hello, World!", 13);
    printf ("  PASSED\n");

    // Test 4: Binary data
    printf ("Test 4: Binary data (256 bytes)\n");
    {
      uint8_t  BinaryData[256];
      for (i = 0; i < 256; i++) {
        BinaryData[i] = (uint8_t)i;
      }
      LLVMFuzzerTestOneInput (BinaryData, sizeof (BinaryData));
    }
    printf ("  PASSED\n");

    // Test 5: Large input
    printf ("Test 5: Large input (64KB)\n");
    {
      uint8_t  *LargeData = (uint8_t *)malloc (65536);
      if (LargeData != NULL) {
        memset (LargeData, 'X', 65536);
        LLVMFuzzerTestOneInput (LargeData, 65536);
        free (LargeData);
      }
    }
    printf ("  PASSED\n");
  }

  printf ("\nAll tests completed.\n");

  OneCryptoFuzzCleanup ();
  return 0;
}
