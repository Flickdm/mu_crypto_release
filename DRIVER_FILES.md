# OneCrypto Driver Files

## Purpose
The **Driver** refers to the OneCrypto Loader Drivers that:
1. Load the OneCrypto binary (the crypto service provider)
2. Consume the OneCryptoProtocol to provide BaseCryptLib services
3. Act as the bridge between platform code and the crypto binary

## Driver Files (CryptoBinPkg/Driver/)

### Core Loader Drivers
- **OneCryptoLoaderDxe.c** - DXE driver implementation
- **OneCryptoLoaderDxe.inf** - DXE driver build configuration
- **OneCryptoLoaderMm.c** - MM (Management Mode) driver implementation
- **OneCryptoLoaderMm.inf** - MM driver build configuration

### Loader Infrastructure
- **OneCryptoLoaderShim.h** - OneCrypto-specific shim header
- **SharedLoaderShim.h** - Common shim definitions (shared with other loaders)
- **PeCoffLib.c** - PE/COFF loader implementation for loading crypto binary
- **PeCoffLib.h** - PE/COFF loader header
- **readme.md** - Driver documentation

## Driver Support Files (CryptoBinPkg/Support/)
- **OneCryptoDxeLoader.inf** - DXE loader package configuration
- **OneCryptoLoaderMm.inf** - MM loader package configuration

## Consumer Library (OneCryptoPkg/Library/OneCryptoLib/)
- **OneCryptoLib.c** - Library that calls OneCryptoProtocol methods
- **OneCryptoLib.inf** - Consumer library build configuration

This library is used by the drivers to actually call the crypto functions via the protocol.

## Build Configuration
- **CryptoBinPkg/CryptoBinPkg.dsc** - Updated to build OneCrypto loaders

## Total Driver Files: 13 files
- 9 in CryptoBinPkg/Driver/
- 2 in CryptoBinPkg/Support/
- 2 in OneCryptoPkg/Library/OneCryptoLib/

## What the Driver Does NOT Include
- The crypto binary itself (OpensslPkg/OneCryptoBin/ and OneCryptoPkg/OneCryptoBin/)
- Protocol definitions (OneCryptoPkg/Include/Protocol/)
- Minimal libraries (OneCryptoPkg/Library/Minimal*)
- BaseCryptLib implementations (OpensslPkg/Library/BaseCryptLib/)
