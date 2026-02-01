# OneCrypto Host Fuzzing Infrastructure

Fuzz testing infrastructure for OneCrypto cryptographic functions using LLVM's libFuzzer
and sanitizers (AddressSanitizer, UndefinedBehaviorSanitizer) on a Linux host.

## Quick Start

```bash
# 1. Install dependencies (Ubuntu/Debian)
./install_fuzz_deps.sh

# 2. Build OneCrypto (from workspace root)
cd /path/to/ONE_CRYPTO
stuart_build -c OneCryptoPkg/PlatformBuild.py

# 3. Build the fuzzer
cd OneCryptoPkg/Test/Fuzz
make fuzzer

# 4. Run the fuzzer
export ONECRYPTO_BIN=../../../Build/OneCryptoPkg/DEBUG_GCC5/X64/OneCryptoPkg/OneCryptoBin/OneCryptoBinSupvMm/OUTPUT/OneCryptoBinSupvMm.efi
ASAN_OPTIONS=detect_leaks=0 ./build/FuzzSha256Host corpus/sha256/
```

## How It Works

This fuzzer **loads production OneCrypto `.efi` binaries at runtime** using a PE/COFF loader,
rather than requiring a separate host build. This ensures we're testing the exact same code
that runs in UEFI.

```
┌─────────────────────────────────────────────────────────────────┐
│                        LibFuzzer                                │
│              (Coverage-guided input generation)                 │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Fuzz Harness (Host)                          │
│                  (FuzzSha256Host.c, etc.)                       │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                 PE/COFF Loader (Host)                           │
│       (Loads .efi, applies relocations, finds CryptoEntry)      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│              OneCrypto Binary (.efi)                            │
│           (Production UEFI binary, unmodified)                  │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│            ONE_CRYPTO_DEPENDENCIES (Host)                       │
│      (malloc, free, localtime, /dev/urandom, no-op debug)       │
└─────────────────────────────────────────────────────────────────┘
```

### Key Technical Detail: Calling Convention

UEFI binaries use the **Microsoft x64 ABI** (args in RCX, RDX, R8, R9) even when compiled
with GCC on Linux. Our host code uses **System V AMD64 ABI** (args in RDI, RSI, RDX, RCX).

All function pointers crossing this boundary use `__attribute__((ms_abi))`:

```c
#define EFIAPI  __attribute__((ms_abi))
```

## Dependencies

### System Requirements

- Linux x86_64
- Clang with libFuzzer support
- Built OneCrypto `.efi` binary

### Installing Dependencies

```bash
# Ubuntu/Debian
./install_fuzz_deps.sh

# Or manually:
sudo apt-get install -y clang libclang-rt-18-dev
```

The `libclang-rt-18-dev` package provides libFuzzer, AddressSanitizer, and UBSan runtimes.

## Building

```bash
cd OneCryptoPkg/Test/Fuzz

# Build fuzzer (with libFuzzer)
make fuzzer

# Build standalone test driver (without libFuzzer, for debugging)
make test_driver

# Clean
make clean
```

## Running the Fuzzer

### Basic Usage

```bash
# Set path to OneCrypto binary
export ONECRYPTO_BIN=/path/to/OneCryptoBinSupvMm.efi

# Disable leak detection (OpenSSL has expected leaks)
export ASAN_OPTIONS=detect_leaks=0

# Run fuzzer with corpus directory
./build/FuzzSha256Host corpus/sha256/
```

### Common Options

```bash
# Run for specific duration (seconds)
./build/FuzzSha256Host corpus/sha256/ -max_total_time=3600

# Limit input size
./build/FuzzSha256Host corpus/sha256/ -max_len=4096

# Parallel fuzzing (multiple jobs)
./build/FuzzSha256Host corpus/sha256/ -jobs=4 -workers=4

# Run specific test case
./build/FuzzSha256Host corpus/sha256/specific_input_file
```

### Example Output

```
PeCoffLoader: Image size: 1425408 bytes
PeCoffLoader: Found export 'CryptoEntry' at RVA 0xb9fa
FuzzInit: Crypto protocol size: 1712 bytes
FuzzInit: Crypto protocol initialized (version 1.0)
INFO: Running with entropic power schedule (0xFF, 100).
INFO: Seed: 1347028639
#2      INITED cov: 46 ft: 47 corp: 1/1b exec/s: 0 rss: 36Mb
#3      NEW    cov: 52 ft: 59 corp: 2/3b lim: 4 exec/s: 0 rss: 36Mb
...
#524288 pulse  cov: 52 ft: 109 corp: 10/211b lim: 4096 exec/s: 262144 rss: 475Mb
```

### Test Driver (No Fuzzing)

For debugging without libFuzzer:

```bash
# Run built-in test vectors
./build/TestSha256

# Test specific input
./build/TestSha256 path/to/input.bin
```

## File Structure

```
OneCryptoPkg/Test/Fuzz/
├── README.md                      # This file
├── Makefile                       # Build system
├── install_fuzz_deps.sh           # Dependency installer
│
├── HostUefiCompat.h               # UEFI types for host (with ms_abi)
├── Library/                       # Stub headers for EDK2 includes
│   ├── BaseCryptLib.h             # RSA_KEY_TAG enum, VA_LIST
│   ├── BaseLib.h                  # Empty stub
│   └── BaseMemoryLib.h            # Empty stub
├── Uefi/                          # Stub headers for UEFI includes
│   ├── UefiBaseType.h             # Empty stub
│   └── UefiSpec.h                 # Empty stub
│
├── PeCoffLoaderHost.c/h           # PE/COFF loader implementation
├── OneCryptoFuzzHarnessHost.c/h   # Harness init + host dependencies
│
├── FuzzSha256Host.c               # SHA-256 fuzz harness
├── FuzzHmacSha256.c               # HMAC-SHA256 fuzz harness (template)
├── StandaloneTestDriver.c         # Test driver (no libFuzzer)
│
├── build/                         # Build outputs
└── corpus/                        # Fuzzing corpus
    └── sha256/                    # SHA-256 corpus
```

### Header Strategy

Instead of maintaining duplicate protocol definitions, the fuzzer uses:

1. **`HostUefiCompat.h`** - Provides UEFI base types with `ms_abi` calling convention
2. **Stub headers** - Empty/minimal stubs in `Library/` and `Uefi/` directories
3. **Real protocol headers** - Includes the actual `OneCrypto.h` from MU_BASECORE

This ensures the fuzzer always uses the same protocol definitions as the production code.

## Adding New Fuzz Harnesses

1. Create a new harness file (e.g., `FuzzRsaHost.c`):

```c
#include "OneCryptoFuzzHarnessHost.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    ONE_CRYPTO_PROTOCOL *Crypto = OneCryptoFuzzGetProtocol();
    if (Crypto == NULL) return 0;

    // Your fuzzing logic here using Crypto->...

    return 0;
}
```

2. Add build target to Makefile
3. Create corpus directory: `mkdir -p corpus/rsa/`

## ONE_CRYPTO_DEPENDENCIES

The host provides these callbacks to the OneCrypto binary:

| Function            | Host Implementation      |
|---------------------|--------------------------|
| `AllocatePool`      | `malloc()`               |
| `FreePool`          | `free()`                 |
| `GetTime`           | `localtime()`            |
| `GetRandomNumber64` | `/dev/urandom`           |
| `DebugPrint`        | no-op (ABI limitation)   |

## Troubleshooting

### "Cannot find OneCrypto binary"

Set `ONECRYPTO_BIN` environment variable:
```bash
export ONECRYPTO_BIN=/path/to/OneCryptoBinSupvMm.efi
```

### Memory leak reports

OpenSSL allocates global state that isn't freed. Disable leak detection:
```bash
ASAN_OPTIONS=detect_leaks=0 ./build/FuzzSha256Host corpus/
```

### Crash in PE/COFF loader

Ensure the binary is built for x64:
```bash
file $ONECRYPTO_BIN
# Should show: PE32+ executable (DLL) ... x86-64
```

### Low coverage

The fuzzer only instruments the harness code, not the loaded binary.
Coverage metrics reflect harness paths, not crypto internals.

## References

- [LLVM libFuzzer](https://llvm.org/docs/LibFuzzer.html)
- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [PE/COFF Specification](https://docs.microsoft.com/en-us/windows/win32/debug/pe-format)
