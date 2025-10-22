## @file
# OneCryptoPkg DSC file
#
# This file provides library mappings for OneCryptoPkg.
#
# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = OneCryptoPkg
  PLATFORM_GUID                  = 36470E85-36F2-4BA0-8CC8-937C7D9FF888
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/OneCryptoPkg
  SUPPORTED_ARCHITECTURES        = X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[LibraryClasses]
  #
  # Library mappings as specified
  #
  UefiBootServicesTableLib       | MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  BaseLib                        | OneCryptoPkg/Library/MinimalBaseLib/MinimalBaseLib.inf # Minimal BaseLib to satisfy dependencies
  BaseMemoryLib                  | OneCryptoPkg/Library/MinimalBaseMemoryLib/MinimalBaseMemoryLib.inf
  PrintLib                       | OneCryptoPkg/Library/MinimalBasePrintLib/MinimalBasePrintLib.inf
  MmServicesTableLib             | Features/MM_SUPV/MmSupervisorPkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  StandaloneMmDriverEntryPoint   | OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
  PcdLib                         | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf # Required for UEFI applications - NULL implementation
  DebugLib                       | MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf # Required for UEFI applications - NULL implementation
  RngLib                         | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # OpensslLib needs this, but actual RNG comes through dependency injection
  FltUsedLib                     | MdePkg/Library/FltUsedLib/FltUsedLib.inf
  BaseCryptLib                   | OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf
  TlsLib                         | OpensslPkg/Library/TlsLib/TlsLib.inf
  OpensslLib                     | OpensslPkg/Library/OpensslLib/OpensslLib.inf
  IntrinsicLib                   | OpensslPkg/Library/IntrinsicLib/IntrinsicLib.inf
  MinimalBaseLib                 | OneCryptoPkg/Library/MinimalBaseLib/MinimalBaseLib.inf
  MinimalBaseMemoryLib           | OneCryptoPkg/Library/MinimalBaseMemoryLib/MinimalBaseMemoryLib.inf
  MinimalBasePrintLib            | OneCryptoPkg/Library/MinimalBasePrintLib/MinimalBasePrintLib.inf
  MinimalStandaloneMmDriverEntryPoint | OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
  SharedCryptoLib                | OneCryptoPkg/Library/SharedCryptoLib/SharedCryptoLib.inf

[Components]
  OneCryptoPkg/Library/MinimalBaseLib/MinimalBaseLib.inf
  OneCryptoPkg/Library/MinimalBaseMemoryLib/MinimalBaseMemoryLib.inf
  OneCryptoPkg/Library/MinimalBasePrintLib/MinimalBasePrintLib.inf
  OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
  OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf
  OpensslPkg/Library/TlsLib/TlsLib.inf
  OneCryptoPkg/SharedCryptoBin/SharedCryptoMmBin.inf
  OneCryptoPkg/Library/SharedCryptoLib/SharedCryptoLib.inf

[BuildOptions]
  *_*_*_CC_FLAGS = -D DISABLE_NEW_DEPRECATED_INTERFACES
  # Disable security features to avoid linker issues with minimal dependencies
  MSFT:*_*_*_CC_FLAGS = /GS-
  MSFT:*_*_*_DLINK_FLAGS = /IGNORE:4217