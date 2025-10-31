# OneCrypto Driver Package

This package provides shared cryptographic drivers for UEFI environments, implementing a dynamic loading architecture for crypto services.

## Overview

The OneCrypto drivers provide a protocol-based interface for cryptographic operations, allowing the crypto implementation to be loaded dynamically at runtime rather than being statically linked into each consumer.

**This project does not directly implement crypto algorithms**. Crypto algorithm implementation is provided by the underlying OpenSSL crypto provider in OpensslPkg. The drivers in this package load and expose those services through well-defined UEFI protocols.

## Architecture

### Components

**OneCryptoLoaderDxe** - DXE phase crypto loader driver
- **File**: `OneCryptoLoaderDxe.c`, `OneCryptoLoaderDxe.inf`
- **Purpose**: Loads and initializes the shared crypto binary for DXE environment
- **Produces**: `gOneCryptoDxeProtocolGuid`
- **Dependencies**: `gEfiMemoryAttributeProtocolGuid`

**OneCryptoLoaderMm** - Management Mode crypto loader driver  
- **File**: `OneCryptoLoaderMm.c`, `OneCryptoLoaderMm.inf`
- **Purpose**: Loads and initializes the shared crypto binary for MM environment
- **Produces**: `gOneCryptoMmProtocolGuid` 
- **Dependencies**: `gOneCryptoPrivateProtocolGuid`

### Supporting Files

**PeCoffLib** - PE/COFF binary loader utilities
- **Files**: `PeCoffLib.c`, `PeCoffLib.h`
- **Purpose**: Provides PE/COFF loading capabilities for the crypto binaries

**SharedLoaderShim** - Common loader interface
- **File**: `SharedLoaderShim.h`
- **Purpose**: Common definitions and interfaces used by both loaders

## Protocol Interface

The drivers expose cryptographic services through well-defined UEFI protocols:

- **gOneCryptoDxeProtocolGuid** - DXE crypto services protocol
- **gOneCryptoMmProtocolGuid** - MM crypto services protocol  
- **gOneCryptoPrivateProtocolGuid** - Internal constructor protocol

These protocols provide access to OpenSSL-based cryptographic functions including:
- HMAC operations
- Hash functions (SHA-1, SHA-256, SHA-384, SHA-512)
- Encryption/Decryption
- Digital signatures
- Random number generation

## Building

To build the OneCrypto drivers:

```bash
python .\SingleFlavorBuild.py CryptoBinPkg -t DEBUG -a x64
```

**Output Binaries:**
- `OneCryptoDxeLoader.efi` - DXE crypto loader driver
- `OneCryptoLoaderMm.efi` - MM crypto loader driver

## Dependencies

**Required Packages:**
- MdePkg - Basic UEFI definitions and libraries
- MdeModulePkg - UEFI module framework  
- OpensslPkg - Crypto protocol definitions and headers
- StandaloneMmPkg - Standalone MM framework
- MmSupervisorPkg - MM supervisor libraries

**Key Libraries:**
- UefiDriverEntryPoint - DXE driver entry point
- StandaloneMmDriverEntryPoint - MM driver entry point
- PeCoffLib - PE/COFF binary handling
- MemoryAllocationLib - Memory management
- CacheMaintenanceLib - Cache operations

## Integration

The OneCrypto drivers are designed to work with:

1. **OpensslPkg** - Provides the actual crypto implementation binary (`OneCryptoMmBin.efi`)
2. **Platform firmware** - Consumes the crypto protocols for security operations
3. **UEFI applications** - Can locate and use crypto services through the protocols

## License

Copyright (c) Microsoft Corporation. All rights reserved.
SPDX-License-Identifier: BSD-2-Clause-Patent
