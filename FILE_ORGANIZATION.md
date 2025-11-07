# OneCrypto File Organization by Purpose

## 1. DRIVER FILES (OneCrypto Loader)
**Purpose:** Load the crypto binary and provide BaseCryptLib services via protocol

### CryptoBinPkg/Driver/ (9 files)
- OneCryptoLoaderDxe.c
- OneCryptoLoaderDxe.inf
- OneCryptoLoaderMm.c
- OneCryptoLoaderMm.inf
- OneCryptoLoaderShim.h
- SharedLoaderShim.h
- PeCoffLib.c
- PeCoffLib.h
- readme.md

### CryptoBinPkg/Support/ (2 files)
- OneCryptoDxeLoader.inf
- OneCryptoLoaderMm.inf

### OneCryptoPkg/Library/OneCryptoLib/ (2 files)
- OneCryptoLib.c (Consumer library that calls the protocol)
- OneCryptoLib.inf

**Total Driver Files: 13**

---

## 2. CRYPTO BINARY FILES (OneCryptoBin Implementation)
**Purpose:** The actual crypto service provider loaded by the driver

### OpensslPkg/OneCryptoBin/ (3 files)
- OneCryptoBin.c
- OneCryptoMmBin.inf
- Scripts/Templates/License.txt

### OneCryptoPkg/OneCryptoBin/ (10 files)
- InternalTlsLib.h
- OneCryptoBin.c
- OneCryptoCrt.c
- OneCryptoMmBin.inf
- Scripts/CreateCryptoProtocol.py
- Scripts/Templates/License.txt
- Scripts/Templates/OneCrypto.template.c
- Scripts/Templates/OneCrypto.template.h
- Scripts/Templates/OneCryptoLib.template.c
- Support/OneCryptoMm.inf

**Total Crypto Binary Files: 13**

---

## 3. PROTOCOL DEFINITIONS
**Purpose:** Define the interface between driver and crypto binary

### OneCryptoPkg/Include/ (9 files)
- Protocol/OneCryptoProtocol.h
- Library/OneCryptoLib.h
- Library/OneCryptoDefinitions.h
- Library/OneCryptoDependencySupport.h
- Library/BaseCryptLib.h
- CrtLibSupport.h

### OpensslPkg/Include/ (11 files)
- Protocol/OneCryptoProtocol.h
- Library/HmacLib.h
- Library/TlsLib.h
- Library/OneCryptoDefinitions.h
- Library/OneCryptoDependencySupport.h
- InternalCryptLib.h
- Private/OneCryptoBaseDebugLib.h
- Private/OneCryptoCrtLibSupport.h
- Private/OneCryptoLibrary.h
- Private/SharedBaseDebugLib.h
- Private/readme.md

**Total Protocol/Header Files: 20**

---

## 4. MINIMAL LIBRARIES (Standalone MM Support)
**Purpose:** Minimal implementations for standalone MM environment

### OneCryptoPkg/Library/ (16 files)
- MinimalBaseLib/MinimalBaseLib.c
- MinimalBaseLib/MinimalBaseLib.inf
- MinimalBaseLib/Math64.c
- MinimalBaseMemoryLib/MinimalBaseMemoryLib.c
- MinimalBaseMemoryLib/MinimalBaseMemoryLib.inf
- MinimalBasePrintLib/MinimalBasePrintLib.c
- MinimalBasePrintLib/MinimalBasePrintLib.inf
- MinimalSafeIntLib/MinimalSafeIntLib.c
- MinimalSafeIntLib/MinimalSafeIntLib.inf
- MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.c
- MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
- MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.uni
- MinimalStandaloneMmDriverEntryPoint/AsmMinimalStandaloneMmDriverEntryPoint.nasm

### OneCryptoPkg/Include/Library/ (4 files)
- MinimalBaseLib.h
- MinimalBaseMemoryLib.h
- MinimalBasePrintLib.h
- MinimalSafeIntLib.h

**Total Minimal Library Files: 20**

---

## 5. BASECRYPTLIB MODERNIZATION
**Purpose:** Clean up and modernize BaseCryptLib structure

### OpensslPkg/Library/BaseCryptLib/ (Added/Modified)
- BaseCryptLib.c (NEW)
- BaseCryptLib.inf (MODIFIED)
- Cipher/CryptAes.c (MODIFIED)
- Hash/CryptSha256.c (MODIFIED)
- Hash/CryptSm3.c (MODIFIED)
- Hash/Readme.md (NEW)
- Hmac/CryptHmac.c (MODIFIED)
- Info/CryptInfo.c (NEW)
- Kdf/CryptHkdf.c (MODIFIED)
- Math/DivS64x64Remainder.c (NEW - but should move to MinimalBaseLib)
- Pem/CryptPem.c (MODIFIED)
- Pk/CryptAuthenticode.c (MODIFIED)
- Pk/CryptPkcs1Oaep.c (MODIFIED)
- Pk/CryptPkcs7Encrypt.c (NEW)
- Pk/CryptPkcs7Sign.c (MODIFIED)
- Rand/CryptRand.c (MODIFIED)
- Rand/CryptRand.h (NEW)
- Syscall/BaseMemAllocation.c (RENAMED from SysCall)
- Syscall/ConstantTimeClock.c (RENAMED from SysCall)
- Syscall/CrtWrapper.c (RENAMED from SysCall)
- Syscall/RuntimeMemAllocation.c (RENAMED from SysCall)
- Syscall/TimerWrapper.c (RENAMED from SysCall)
- Syscall/inet_pton.c (NEW)
- BN/CryptBn.c (RENAMED from Bn)
- Tls/TlsConfig.c (MOVED from TlsLib)
- Tls/TlsInit.c (MOVED from TlsLib)
- Tls/TlsProcess.c (MOVED from TlsLib)

### Deleted (Clean up)
- Hash/CryptCShake256.c
- Hash/CryptDispatchApDxe.c
- Hash/CryptDispatchApMm.c
- Hash/CryptDispatchApPei.c
- Hash/CryptParallelHash.c
- Hash/CryptParallelHash.h
- Hash/CryptSha3.c
- Hash/CryptXkcp.c
- Pk/CryptPkcs7VerifyEkuRuntime.c
- Rand/CryptRandTsc.c
- SysCall/UnitTestHostCrtWrapper.c

**Total BaseCryptLib Files: ~40**

---

## 6. OPENSSL SUPPORT FILES
**Purpose:** Support OneCrypto binary with OpenSSL integration

### OpensslPkg/Library/Library/Include/ (28 CRT headers)
- CrtLibSupport.h
- arpa/inet.h, arpa/nameser.h
- assert.h, ctype.h, errno.h, fcntl.h, intrin.h
- limits.h, memory.h
- netinet/in.h
- openssl/opensslconf.h
- stdarg.h, stddef.h, stdio.h, stdlib.h, string.h, strings.h
- sys/param.h, sys/shm.h, sys/socket.h, sys/syscall.h
- sys/time.h, sys/types.h, sys/utsname.h
- syslog.h, time.h, unistd.h

### OpensslPkg/Library/ (8 files)
- Include/CrtLibSupport.h (MODIFIED)
- Include/InternalTlsLib.h (NEW)
- IntrinsicLib/CopyMem.c (MODIFIED)
- IntrinsicLib/MemoryIntrinsics.c (MODIFIED)
- MemoryIntrinsicLib/MemoryIntrinsicLib.inf (NEW)
- MemoryIntrinsicLib/MemoryIntrinsics.c (NEW)
- OpensslLib/OpenssLibShared.inf (NEW)
- OpensslLib/OpensslLibOneCrypto.inf (NEW)
- OpensslLib/*.inf (MODIFIED - 5 files)
- OpensslLib/OpensslStub/rand_pool.c (MODIFIED)

**Total OpenSSL Support Files: ~44**

---

## 7. PACKAGE CONFIGURATION
**Purpose:** Package definitions and build configuration

### OneCryptoPkg/ (3 files)
- OneCryptoPkg.dec
- OneCryptoPkg.dsc
- README.md

### CryptoBinPkg/ (1 file)
- CryptoBinPkg.dsc (MODIFIED)

### OpensslPkg/ (2 files)
- OpensslPkg.dec (MODIFIED)
- OpensslPkg.dsc (MODIFIED)

**Total Package Config Files: 6**

---

## 8. DOCUMENTATION
**Purpose:** Architecture and integration documentation

### Docs/ (5 files)
- Architecture.md
- CreateCryptoProtocol.md
- PlatformIntegration.md
- README.md
- RngSecurityModel.md

### OneCryptoPkg/OneCryptoBin/ (2 files)
- OneCryptoPkgDoxygen.config
- README_DOCS.md

**Total Documentation Files: 7**

---

## 9. BUILD & INFRASTRUCTURE
**Purpose:** Build scripts and configuration

### Root Level (8 files)
- .gitattributes (NEW)
- .gitignore (MODIFIED)
- .gitmodules (MODIFIED)
- CommonBuildSettings.py (MODIFIED)
- MultiFlavorBuild.py (MODIFIED)
- Readme.rst (MODIFIED)
- SingleFlavorBuild.py (MODIFIED)
- pip-requirements.txt (MODIFIED)

### OpensslPkg/ (1 file)
- OpensslPkg.ci.yaml (MODIFIED)

**Total Build Files: 9**

---

## SUMMARY BY PURPOSE

1. **Driver (Loader)**: 13 files
2. **Crypto Binary**: 13 files  
3. **Protocol Definitions**: 20 files
4. **Minimal Libraries**: 20 files
5. **BaseCryptLib Modernization**: ~40 files
6. **OpenSSL Support**: ~44 files
7. **Package Configuration**: 6 files
8. **Documentation**: 7 files
9. **Build & Infrastructure**: 9 files

**Total: ~172 files changed**

---

## RECOMMENDED PR SPLIT

### PR1: BaseCryptLib Modernization (Foundation)
- BaseCryptLib reorganization
- TLS consolidation
- Directory case fixes
- Clean up deprecated files
- ~40 files

### PR2: OneCryptoPkg Foundation (Protocol & Minimal Libraries)
- OneCryptoPkg package structure
- Protocol definitions
- Minimal libraries
- ~46 files

### PR3: OneCrypto Binary Implementation
- OneCryptoBin implementation
- OpenSSL integration files
- CRT headers
- Code generation scripts
- ~57 files

### PR4: OneCrypto Loader Drivers
- Loader drivers (DXE & MM)
- OneCryptoLib consumer library
- PE/COFF loader
- ~13 files

### PR5: Documentation & Build
- Documentation
- Build script updates
- Doxygen configuration
- ~16 files
