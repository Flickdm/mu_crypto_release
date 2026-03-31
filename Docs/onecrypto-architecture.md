# OneCrypto Architecture (mu_crypto_release)

This document summarizes OneCrypto component boundaries in this repository.

## Design Intent

OneCrypto centralizes cryptographic service implementation behind a protocol so
consumers use stable BaseCryptLib/TlsLib APIs instead of direct protocol calls.

## Core Components

### Protocol Contract

- `MU_BASECORE/CryptoPkg/Include/Protocol/OneCrypto.h`
- `MU_BASECORE/CryptoPkg/Include/Library/BaseCryptLib.h`

Responsibilities:

- define protocol function pointer contract
- define protocol version (`ONE_CRYPTO_VERSION_MAJOR`/`MINOR`)
- define public library API signatures consumed by platform code

### Producer (OneCrypto Binary)

- `OneCryptoPkg/OneCryptoBin/OneCryptoBin.c`

Responsibilities:

- initialize protocol version
- map protocol function pointers in `CryptoInit()`
- publish runtime crypto services through OneCrypto binaries

### Consumer (BaseCryptLibOnOneCrypto)

- `MU_BASECORE/CryptoPkg/Library/BaseCryptLibOnOneCrypto/OneCryptoLib.c`
- `MU_BASECORE/CryptoPkg/Library/BaseCryptLibOnOneCrypto/DxeCryptLib.c`
- `MU_BASECORE/CryptoPkg/Library/BaseCryptLibOnOneCrypto/SmmCryptLib.c`
- `MU_BASECORE/CryptoPkg/Library/BaseCryptLibOnOneCrypto/StandaloneMmCryptLib.c`

Responsibilities:

- expose BaseCryptLib/TlsLib-compatible wrappers
- locate protocol in phase-specific constructors
- enforce version-gated dispatch with consumer-side error handling

### Loaders and Binary Packaging

- `OneCryptoPkg/OneCryptoLoaders/*`
- `OneCryptoPkg/OneCryptoBin/*.inf`
- `OneCryptoPkg/OneCryptoPkg.dec`

Responsibilities:

- declare producer/consumer relationships via INF/DEC
- support phase-specific integration and packaging

## Update Boundaries

For protocol/API changes, update these in lockstep:

1. protocol contract (`OneCrypto.h`, and `BaseCryptLib.h` if public API changes)
2. producer wiring (`OneCryptoBin.c`)
3. consumer wrappers (`BaseCryptLibOnOneCrypto`)
4. phase integration declarations (INF/DEC) when applicable

## Build Validation (OneCryptoPkg)

```bash
stuart_setup -c .pytool/CISettings.py -p OneCryptoPkg
stuart_update -c .pytool/CISettings.py -p OneCryptoPkg
stuart_ci_build -c .pytool/CISettings.py -p OneCryptoPkg -t DEBUG TOOL_CHAIN_TAG=CLANGPDB
stuart_ci_build -c .pytool/CISettings.py -p OneCryptoPkg -t RELEASE TOOL_CHAIN_TAG=CLANGPDB
```

## External Reference

Canonical update guidance:

- <https://github.com/microsoft/mu_basecore/blob/release/202511/CryptoPkg/Library/BaseCryptLibOnOneCrypto/Readme.md>
