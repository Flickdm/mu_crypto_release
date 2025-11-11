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
  BaseLib                        | MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib                  | MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PrintLib                       | MdePkg/Library/BasePrintLib/BasePrintLib.inf
  MmServicesTableLib             | MdePkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  StandaloneMmDriverEntryPoint   | OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
  PcdLib                         | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf # Required for UEFI applications - NULL implementation
  DebugLib                       | MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf # Required for UEFI applications - NULL implementation
  RngLib                         | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # OpensslLib needs this, but actual RNG comes through dependency injection
  RegisterFilterLib              | MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
  SafeIntLib                     | OneCryptoPkg/Library/MinimalSafeIntLib/MinimalSafeIntLib.inf
  FltUsedLib                     | MdePkg/Library/FltUsedLib/FltUsedLib.inf
  BaseCryptLib                   | OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf
  OpensslLib                     | OpensslPkg/Library/OpensslLib/OpenssLibShared.inf
  IntrinsicLib                   | OpensslPkg/Library/IntrinsicLib/IntrinsicLib.inf
  MinimalStandaloneMmDriverEntryPoint | OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf

[Components]
  OneCryptoPkg/Library/MinimalSafeIntLib/MinimalSafeIntLib.inf
  OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
  OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf
  OneCryptoPkg/OneCryptoBin/OneCryptoMmBin.inf

[BuildOptions]
  *_*_*_CC_FLAGS = -D DISABLE_NEW_DEPRECATED_INTERFACES
  # Disable security features to avoid linker issues with minimal dependencies
  MSFT:*_*_*_CC_FLAGS = /GS-
  MSFT:*_*_*_DLINK_FLAGS = /IGNORE:4217
