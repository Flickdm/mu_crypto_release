# OneCrypto PR Breakdown

## Overview
The OneCrypto architecture has been split into 5 logical PRs that can be reviewed and merged sequentially. Each PR builds upon the previous one.

## PR Sequence

### PR1: BaseCryptLib Modernization & Infrastructure
**Branch:** `pr/basecryptlib-modernization`  
**Base:** `origin/dev/openssl-3.0.15`  
**Purpose:** Clean up and modernize BaseCryptLib, prepare foundation for OneCrypto

**Commits (8):**
- 5e34092 Infrastructure and configuration updates
- 07bdbe5 Update OpensslLib to remove flavor references
- 45eaf70 Move TLS implementation files to BaseCryptLib/Tls/
- 905b911 Consolidate TLS into BaseCryptLib and add shared header
- 669227e Add new implementations and update BaseCryptLib configuration
- 060419d Update BaseCryptLib implementations
- ac990a0 Fix directory case sensitivity for BaseCryptLib
- ac1509c Add SafeIntLib dependency to BaseCryptLib

**Key Changes:**
- TLS consolidation into BaseCryptLib
- Directory case fixes (Bn→BN, SysCall→Syscall)
- SafeIntLib integration
- Removal of NULL stubs and legacy code

---

### PR2: OneCryptoPkg Foundation (Protocol, Minimal Libraries)
**Branch:** `pr/onecryptopkg-foundation`  
**Base:** `pr/basecryptlib-modernization`  
**Purpose:** Add OneCryptoPkg package with protocol definitions and minimal standalone MM libraries

**Commits (9):**
- caf2c9c Add OneCryptoPkg package structure
- 0642620 Add OneCrypto protocol definitions and headers
- af253db Add minimal library implementations for standalone MM
- 81e6600 Remove incorrect dependencies from InternalTlsLib.h
- d6f6d54 Add MinimalSafeIntLib and update InternalTlsLib.h dependencies
- 1717b9f Use MinimalSafeIntLib in OneCryptoPkg.dsc
- 6ed51d1 Fix MinimalSafeIntLib LIBRARY_CLASS declaration
- a46138d Complete MinimalBaseMemoryLib header implementation
- ed68d6e Add documentation comments to IntrinsicLib and update MinimalBaseMemoryLib header

**Key Changes:**
- OneCryptoPkg.dec/.dsc package files
- OneCryptoProtocol.h interface definition
- MinimalBaseLib, MinimalBaseMemoryLib, MinimalBasePrintLib
- MinimalSafeIntLib implementation
- MinimalStandaloneMmDriverEntryPoint

---

### PR3: OneCryptoBin Implementation (Crypto Binary)
**Branch:** `pr/onecryptobin-implementation`  
**Base:** `pr/onecryptopkg-foundation`  
**Purpose:** Implement the OneCrypto binary that provides crypto services via protocol

**Commits (4):**
- 28de6e7 Add OneCryptoBin protocol implementation
- d78cbf5 Add OpensslPkg updates for OneCrypto support
- d1e4dc8 fixup! Add OpensslPkg updates for OneCrypto support (rand_pool.c TODO fixes)
- ba0b154 Add SafeIntLib dependency to OneCryptoMmBin

**Key Changes:**
- OneCryptoBin.c - Protocol implementation
- OneCryptoMmBin.inf - Build configuration
- CreateCryptoProtocol.py - Code generator for protocol wrappers
- Math64.c moved to MinimalBaseLib
- OpensslLib variants (OpenssLibShared.inf, OpensslLibOneCrypto.inf)
- MemoryIntrinsicLib for OpenSSL
- CRT headers for crypto libraries

**New Files:**
- OpensslPkg/OneCryptoBin/ - Complete crypto binary implementation
- OneCryptoPkg/OneCryptoBin/ - Protocol generation scripts
- OpensslPkg/Library/Library/Include/ - CRT headers

---

### PR4: OneCrypto Loader Drivers
**Branch:** `pr/onecrypto-loaders`  
**Base:** `pr/onecryptobin-implementation`  
**Purpose:** Add loader drivers that consume OneCrypto protocol and provide BaseCryptLib services

**Commits (6):**
- 8f181d4 Add OneCryptoLib consumer library
- 83eef3c Add OneCrypto loader drivers to CryptoBinPkg
- 9dd65be Add OneCrypto loader infrastructure
- 49b52d5 Update CryptoBinPkg.dsc for OneCrypto architecture
- 5c585bc Update build scripts for new architecture
- a45ab3f Add gitattributes to preserve CRLF for yaml file

**Key Changes:**
- OneCryptoLib.c - Consumer library that calls protocol
- OneCryptoLoaderDxe.c/inf - DXE loader driver
- OneCryptoLoaderMm.c/inf - MM loader driver
- PeCoffLib - PE/COFF loader for crypto binary
- Updated CryptoBinPkg.dsc (272→98 lines)
- Build script updates

---

### PR5: Documentation & Build Information
**Branch:** `pr/onecrypto-documentation`  
**Base:** `pr/onecrypto-loaders`  
**Purpose:** Add comprehensive documentation and build information

**Commits (6):**
- 0a2a9dd Add CryptInfo.c for version and build information
- 7ee921c Document MinimalBaseMemoryLib and MinimalBasePrintLib origins
- f7cd19d Add OneCrypto platform integration documentation
- c843a7a Add centralized documentation in Docs/ directory
- 6e5a3c3 Add comprehensive OneCrypto architecture documentation with ASCII diagrams
- f5e57e1 Add Doxygen configuration and documentation build instructions

**Key Changes:**
- Docs/Architecture.md - Complete architecture overview
- Docs/PlatformIntegration.md - Integration guide
- Docs/CreateCryptoProtocol.md - Code generator documentation
- Docs/RngSecurityModel.md - Security model documentation
- OneCryptoPkgDoxygen.config - API documentation generation
- CryptInfo.c - Build version information

---

## Merge Strategy

1. **PR1** → Merge into `dev/openssl-3.0.15`
   - Standalone, no dependencies on later PRs
   - Modernizes existing code

2. **PR2** → Merge after PR1
   - Depends on: PR1 (BaseCryptLib modernization)
   - Adds new package infrastructure

3. **PR3** → Merge after PR2
   - Depends on: PR2 (Protocol definitions, minimal libraries)
   - Adds crypto binary implementation

4. **PR4** → Merge after PR3
   - Depends on: PR3 (Crypto binary must exist)
   - Adds loader drivers that consume the binary

5. **PR5** → Merge after PR4 (or can be merged independently)
   - Documentation-only, minimal code impact
   - Could be merged in parallel with other PRs

## Build Verification

Each branch should build successfully:
```powershell
# PR1
python SingleFlavorBuild.py OpensslPkg

# PR2
python SingleFlavorBuild.py OneCryptoPkg

# PR3
python SingleFlavorBuild.py OneCryptoPkg

# PR4
python SingleFlavorBuild.py CryptoBinPkg

# PR5
python SingleFlavorBuild.py CryptoBinPkg
doxygen OneCryptoPkg/OneCryptoBin/OneCryptoPkgDoxygen.config
```

## Notes

- All fixup commits have been squashed into their parent commits during cherry-pick
- Each PR is buildable and testable independently
- PRs maintain logical separation of concerns:
  - PR1: Foundation cleanup
  - PR2: New package infrastructure
  - PR3: Binary implementation
  - PR4: Consumer drivers
  - PR5: Documentation

## Review Focus Areas

**PR1:** Architecture cleanup, directory structure  
**PR2:** Protocol design, minimal library implementation  
**PR3:** Crypto binary implementation, OpenSSL integration  
**PR4:** Loader architecture, PE/COFF loading  
**PR5:** Documentation completeness and accuracy
