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
  SUPPORTED_ARCHITECTURES        = IA32|X64|AARCH64
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

[Components]
  OneCryptoPkg/Library/MinimalBaseLib/MinimalBaseLib.inf
  OneCryptoPkg/Library/MinimalBaseMemoryLib/MinimalBaseMemoryLib.inf
  OneCryptoPkg/Library/MinimalBasePrintLib/MinimalBasePrintLib.inf
  OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf
  OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf