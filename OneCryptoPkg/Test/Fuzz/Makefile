# OneCrypto Fuzzing Makefile
#
# This Makefile builds fuzzing harnesses for OneCrypto using libFuzzer.
# It loads the existing OneCrypto PE/COFF binary at runtime and provides
# host implementations of the UEFI dependencies.
#
# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# Usage:
#   make              - Show info and available targets
#   make test_compile - Test syntax of host-compatible harness files
#   make clean        - Clean build artifacts
#

# Compiler settings
CC := clang
CXX := clang++

# Build directories
BUILD_DIR := build
CORPUS_DIR := corpus

# Base paths - relative to this Makefile's directory
WORKSPACE := $(realpath ../../..)
FUZZ_DIR := $(realpath .)

# Path to the built OneCrypto binary (built via stuart_build)
# Update this path based on your build configuration
ONECRYPTO_BIN := $(WORKSPACE)/Build/OneCryptoPkg/DEBUG_GCC5/X64/OneCryptoPkg/OneCryptoBin/OneCryptoBinSupvMm/OUTPUT/OneCryptoBinSupvMm.efi

# Compiler flags for host-compatible fuzzing (no UEFI headers)
# The host headers provide all necessary type definitions
CFLAGS := \
	-g -O1 \
	-fno-omit-frame-pointer \
	-DHOST_APPLICATION \
	-I$(FUZZ_DIR)

# Check if libFuzzer is available
FUZZER_CHECK := $(shell echo 'int LLVMFuzzerTestOneInput(const char *d, long s) { return 0; }' | \
	$(CC) -x c - -fsanitize=fuzzer -o /dev/null 2>/dev/null && echo "yes" || echo "no")

ifeq ($(FUZZER_CHECK),yes)
  HAVE_FUZZER := 1
  # Fuzzing-specific flags (for building with libFuzzer)
  FUZZ_CFLAGS := $(CFLAGS) -fsanitize=fuzzer,address,undefined
  FUZZ_LDFLAGS := -fsanitize=fuzzer,address,undefined
else
  HAVE_FUZZER := 0
  # Build without fuzzer - just address/undefined sanitizers
  FUZZ_CFLAGS := $(CFLAGS) -fsanitize=address,undefined
  FUZZ_LDFLAGS := -fsanitize=address,undefined
endif

# Warning flags
WARNINGS := -Wall -Wextra -Wno-unused-parameter

# Linker flags for fuzzing
FUZZ_LDFLAGS := -fsanitize=fuzzer,address,undefined

# Host-compatible source files
HARNESS_HOST_COMMON := OneCryptoFuzzHarnessHost.c
FUZZ_SHA256_HOST := FuzzSha256Host.c

# Default target
.PHONY: all
all: $(BUILD_DIR) info

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(CORPUS_DIR)/sha256

.PHONY: info
info:
	@echo "========================================"
	@echo "OneCrypto Fuzzer Build System"
	@echo "========================================"
	@echo ""
	@echo "This is the host-compatible fuzzing infrastructure for OneCrypto."
	@echo "It uses standalone type definitions that don't require UEFI headers."
	@echo ""
	@echo "Architecture:"
	@echo "  The OneCrypto binary is phase-agnostic - it only depends on the"
	@echo "  ONE_CRYPTO_DEPENDENCIES interface (malloc, free, time, rng, debug)."
	@echo ""
	@echo "  For fuzzing, we have two approaches:"
	@echo ""
	@echo "  1. PE/COFF Loading (runtime loading of .efi binary)"
	@echo "     - Load existing OneCryptoBinSupvMm.efi at runtime"
	@echo "     - Parse PE/COFF, find CryptoEntry export"
	@echo "     - Pro: Reuses exact production binary"
	@echo "     - Con: No sanitizer instrumentation in crypto code"
	@echo ""
	@echo "  2. Direct Compilation (compile crypto sources for host)"
	@echo "     - Compile OneCryptoBin.c + OpenSSL + BaseCryptLib for host"
	@echo "     - Link directly with fuzzer harness"
	@echo "     - Pro: Full sanitizer coverage"
	@echo "     - Con: Complex build, different binary than production"
	@echo ""
	@echo "Current Files:"
	@echo "  HostUefiCompat.h         - UEFI-compatible types for host (ms_abi)"
	@echo "  Library/BaseCryptLib.h   - Stub for real OneCrypto.h"
	@echo "  OneCryptoFuzzHarnessHost.c/h - Common harness infrastructure"
	@echo "  FuzzSha256Host.c         - SHA-256 fuzzer harness"
	@echo ""
	@echo "Available targets:"
	@echo "  make info           - Show this information"
	@echo "  make test_compile   - Test compilation of host harness files"
	@echo "  make objects        - Build object files (needs OneCryptoBin to link)"
	@echo "  make clean          - Remove build artifacts"
	@echo ""
	@echo "OneCrypto Binary: $(ONECRYPTO_BIN)"
	@test -f "$(ONECRYPTO_BIN)" && echo "  Status: FOUND" || echo "  Status: NOT FOUND (run stuart_build first)"

# Test that the host-compatible harness files compile (syntax check only)
.PHONY: test_compile
test_compile: $(BUILD_DIR)
	@echo "Testing compilation of host-compatible fuzzer harness files..."
	@echo ""
	@$(CC) $(CFLAGS) $(WARNINGS) -fsyntax-only HostUefiCompat.h && \
		echo "✓ HostUefiCompat.h syntax OK" || \
		echo "✗ HostUefiCompat.h has errors"
	@$(CC) $(CFLAGS) $(WARNINGS) -fsyntax-only OneCryptoFuzzHarnessHost.h && \
		echo "✓ OneCryptoFuzzHarnessHost.h syntax OK" || \
		echo "✗ OneCryptoFuzzHarnessHost.h has errors"
	@$(CC) $(CFLAGS) $(WARNINGS) -fsyntax-only -c OneCryptoFuzzHarnessHost.c -o /dev/null && \
		echo "✓ OneCryptoFuzzHarnessHost.c syntax OK" || \
		echo "✗ OneCryptoFuzzHarnessHost.c has errors"
	@$(CC) $(CFLAGS) $(WARNINGS) -fsyntax-only -c FuzzSha256Host.c -o /dev/null && \
		echo "✓ FuzzSha256Host.c syntax OK" || \
		echo "✗ FuzzSha256Host.c has errors"
	@echo ""
	@echo "Note: Full linking requires OneCryptoBinHost to be built or PE/COFF loader"

# Compile object files for fuzzer (with libFuzzer instrumentation)
$(BUILD_DIR)/PeCoffLoaderHost.o: PeCoffLoaderHost.c PeCoffLoaderHost.h HostUefiCompat.h
	$(CC) $(FUZZ_CFLAGS) $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/OneCryptoFuzzHarnessHost.o: OneCryptoFuzzHarnessHost.c OneCryptoFuzzHarnessHost.h PeCoffLoaderHost.h HostUefiCompat.h
	$(CC) $(FUZZ_CFLAGS) $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/FuzzSha256Host.o: FuzzSha256Host.c OneCryptoFuzzHarnessHost.h HostUefiCompat.h
	$(CC) $(FUZZ_CFLAGS) $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/FuzzPkcs7VerifyHost.o: FuzzPkcs7VerifyHost.c OneCryptoFuzzHarnessHost.h HostUefiCompat.h
	$(CC) $(FUZZ_CFLAGS) $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/FuzzAuthenticodeVerifyHost.o: FuzzAuthenticodeVerifyHost.c OneCryptoFuzzHarnessHost.h HostUefiCompat.h
	$(CC) $(FUZZ_CFLAGS) $(WARNINGS) -c $< -o $@

# Compile object files for standalone test (without libFuzzer)
$(BUILD_DIR)/PeCoffLoaderHost_nofuzz.o: PeCoffLoaderHost.c PeCoffLoaderHost.h HostUefiCompat.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/OneCryptoFuzzHarnessHost_nofuzz.o: OneCryptoFuzzHarnessHost.c OneCryptoFuzzHarnessHost.h PeCoffLoaderHost.h HostUefiCompat.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/FuzzSha256Host_nofuzz.o: FuzzSha256Host.c OneCryptoFuzzHarnessHost.h HostUefiCompat.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/FuzzPkcs7VerifyHost_nofuzz.o: FuzzPkcs7VerifyHost.c OneCryptoFuzzHarnessHost.h HostUefiCompat.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/FuzzAuthenticodeVerifyHost_nofuzz.o: FuzzAuthenticodeVerifyHost.c OneCryptoFuzzHarnessHost.h HostUefiCompat.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/StandaloneTestDriver.o: StandaloneTestDriver.c OneCryptoFuzzHarnessHost.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined $(WARNINGS) -c $< -o $@

# Build the complete fuzzer executable
$(BUILD_DIR)/FuzzSha256Host: $(BUILD_DIR)/FuzzSha256Host.o $(BUILD_DIR)/OneCryptoFuzzHarnessHost.o $(BUILD_DIR)/PeCoffLoaderHost.o
ifeq ($(HAVE_FUZZER),1)
	$(CC) $(FUZZ_LDFLAGS) $^ -o $@
	@echo ""
	@echo "Fuzzer built: $@"
	@echo ""
	@echo "To run, set ONECRYPTO_BIN or build the OneCrypto binary first:"
	@echo "  export ONECRYPTO_BIN=/path/to/OneCryptoBinSupvMm.efi"
	@echo "  $@ $(CORPUS_DIR)/sha256/"
else
	@echo ""
	@echo "ERROR: libFuzzer not available."
	@echo "Install with: sudo apt install clang libfuzzer-18-dev"
	@echo ""
	@echo "Alternatively, build the standalone test driver:"
	@echo "  make test_driver"
	@false
endif

$(BUILD_DIR)/FuzzPkcs7VerifyHost: $(BUILD_DIR)/FuzzPkcs7VerifyHost.o $(BUILD_DIR)/OneCryptoFuzzHarnessHost.o $(BUILD_DIR)/PeCoffLoaderHost.o
ifeq ($(HAVE_FUZZER),1)
	$(CC) $(FUZZ_LDFLAGS) $^ -o $@
	@echo "Fuzzer built: $@"
else
	@echo "ERROR: libFuzzer not available."
	@false
endif

$(BUILD_DIR)/FuzzAuthenticodeVerifyHost: $(BUILD_DIR)/FuzzAuthenticodeVerifyHost.o $(BUILD_DIR)/OneCryptoFuzzHarnessHost.o $(BUILD_DIR)/PeCoffLoaderHost.o
ifeq ($(HAVE_FUZZER),1)
	$(CC) $(FUZZ_LDFLAGS) $^ -o $@
	@echo "Fuzzer built: $@"
else
	@echo "ERROR: libFuzzer not available."
	@false
endif

# Build standalone test driver (doesn't require libFuzzer)
$(BUILD_DIR)/TestSha256: $(BUILD_DIR)/FuzzSha256Host_nofuzz.o $(BUILD_DIR)/OneCryptoFuzzHarnessHost_nofuzz.o $(BUILD_DIR)/PeCoffLoaderHost_nofuzz.o $(BUILD_DIR)/StandaloneTestDriver.o
	$(CC) -fsanitize=address,undefined $^ -o $@
	@echo ""
	@echo "Standalone test driver built: $@"
	@echo ""
	@echo "To run:"
	@echo "  export ONECRYPTO_BIN=/path/to/OneCryptoBinSupvMm.efi"
	@echo "  $@"

.PHONY: test_driver
test_driver: $(BUILD_DIR) $(BUILD_DIR)/TestSha256

.PHONY: fuzz_sha256
fuzz_sha256: $(BUILD_DIR)/FuzzSha256Host
	@echo "Running SHA256 fuzzer..."
	@mkdir -p $(CORPUS_DIR)/sha256
	$(BUILD_DIR)/FuzzSha256Host $(CORPUS_DIR)/sha256/

.PHONY: fuzz_pkcs7
fuzz_pkcs7: $(BUILD_DIR)/FuzzPkcs7VerifyHost
	@echo "Running PKCS#7 Verify fuzzer..."
	@mkdir -p $(CORPUS_DIR)/pkcs7
	$(BUILD_DIR)/FuzzPkcs7VerifyHost $(CORPUS_DIR)/pkcs7/

.PHONY: fuzz_authenticode
fuzz_authenticode: $(BUILD_DIR)/FuzzAuthenticodeVerifyHost
	@echo "Running Authenticode Verify fuzzer..."
	@mkdir -p $(CORPUS_DIR)/authenticode
	$(BUILD_DIR)/FuzzAuthenticodeVerifyHost $(CORPUS_DIR)/authenticode/

.PHONY: objects
objects: $(BUILD_DIR) $(BUILD_DIR)/PeCoffLoaderHost.o $(BUILD_DIR)/OneCryptoFuzzHarnessHost.o $(BUILD_DIR)/FuzzSha256Host.o
	@echo "Object files built in $(BUILD_DIR)/"

.PHONY: fuzzer
fuzzer: $(BUILD_DIR)/FuzzSha256Host

.PHONY: fuzzers
fuzzers: $(BUILD_DIR)/FuzzSha256Host $(BUILD_DIR)/FuzzPkcs7VerifyHost $(BUILD_DIR)/FuzzAuthenticodeVerifyHost
	@echo ""
	@echo "All fuzzers built:"
	@echo "  $(BUILD_DIR)/FuzzSha256Host"
	@echo "  $(BUILD_DIR)/FuzzPkcs7VerifyHost"
	@echo "  $(BUILD_DIR)/FuzzAuthenticodeVerifyHost"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: help
help: info
