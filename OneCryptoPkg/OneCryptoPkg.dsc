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


[Components]

  #############################################################################
  ## OneCryptoBin START
  #############################################################################

  ## OneCryptBin meant for SupvMm
  #
  # This binary provides the crypto for a SupvMm based platform.
  ##
  OneCryptoPkg/OneCryptoBin/OneCryptoBinSupvMm.inf {
    <LibraryClasses>
      UefiBootServicesTableLib       | MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
      BaseLib                        | MdePkg/Library/BaseLib/BaseLib.inf
      BaseMemoryLib                  | MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
      PrintLib                       | MdePkg/Library/BasePrintLib/BasePrintLib.inf
      PcdLib                         | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf # Required for UEFI applications - NULL implementation
      RngLib                         | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # OpensslLib needs this, but actual RNG comes through dependency injection
      RegisterFilterLib              | MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
      SafeIntLib                     | MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
      FltUsedLib                     | MdePkg/Library/FltUsedLib/FltUsedLib.inf
      StackCheckLib                  | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      StackCheckFailureHookLib       | MdePkg/Library/StackCheckFailureHookLibNull/StackCheckFailureHookLibNull.inf
      BaseCryptLib                   | OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf
      OpensslLib                     | OpensslPkg/Library/OpensslLib/OpenssLibShared.inf
      IntrinsicLib                   | OpensslPkg/Library/IntrinsicLib/IntrinsicLib.inf
      BaseCryptCrtLib                | OneCryptoPkg/Library/BaseCryptCrtLibOneCrypto/BaseCryptCrtLib.inf
      DebugLib                       | OneCryptoPkg/Library/BaseDebugLibOneCrypto/BaseDebugLibOneCrypto.inf # OneCrypto-specific DebugLib using dependency injection
      StandaloneMmDriverEntryPoint   | MmSupervisorPkg/Library/StandaloneMmDriverEntryPoint/StandaloneMmDriverEntryPoint.inf
      MmServicesTableLib             | MmSupervisorPkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  }

  ## OneCryptBin meant for StandaloneMm
  #
  # This binary provides the crypto for a StandaloneMm based platform.
  ##
  OneCryptoPkg/OneCryptoBin/OneCryptoBinStandaloneMm.inf {
    <LibraryClasses>
      UefiBootServicesTableLib       | MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
      BaseLib                        | MdePkg/Library/BaseLib/BaseLib.inf
      BaseMemoryLib                  | MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
      PrintLib                       | MdePkg/Library/BasePrintLib/BasePrintLib.inf
      PcdLib                         | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf # Required for UEFI applications - NULL implementation
      RngLib                         | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # OpensslLib needs this, but actual RNG comes through dependency injection
      RegisterFilterLib              | MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
      SafeIntLib                     | MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
      FltUsedLib                     | MdePkg/Library/FltUsedLib/FltUsedLib.inf
      StackCheckLib                  | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      StackCheckFailureHookLib       | MdePkg/Library/StackCheckFailureHookLibNull/StackCheckFailureHookLibNull.inf
      BaseCryptLib                   | OpensslPkg/Library/BaseCryptLib/BaseCryptLib.inf
      OpensslLib                     | OpensslPkg/Library/OpensslLib/OpenssLibShared.inf
      IntrinsicLib                   | OpensslPkg/Library/IntrinsicLib/IntrinsicLib.inf
      BaseCryptCrtLib                | OneCryptoPkg/Library/BaseCryptCrtLibOneCrypto/BaseCryptCrtLib.inf
      DebugLib                       | OneCryptoPkg/Library/BaseDebugLibOneCrypto/BaseDebugLibOneCrypto.inf # OneCrypto-specific DebugLib using dependency injection
      StandaloneMmDriverEntryPoint   | MdePkg/Library/StandaloneMmDriverEntryPoint/StandaloneMmDriverEntryPoint.inf
      MmServicesTableLib             | MdePkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
  }

  #############################################################################
  ## OneCryptoBin END
  #############################################################################

  #############################################################################
  ## OneCryptoLoader START
  #############################################################################

  OneCryptoPkg/OneCryptoLoaders/OneCryptoLoaderDxe.inf {
    <LibraryClasses>
      BaseLib                     | MdePkg/Library/BaseLib/BaseLib.inf
      BaseMemoryLib               | MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
      DebugLib                    | MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      PcdLib                      | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      PrintLib                    | MdePkg/Library/BasePrintLib/BasePrintLib.inf
      UefiLib                     | MdePkg/Library/UefiLib/UefiLib.inf
      UefiRuntimeServicesTableLib | MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
      DevicePathLib               | MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
      RngLib                      | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # Drivers should use the protocol, GetRandomNumber64 will not work.
      IoLib                       | MdePkg/Library/DxeIoLibCpuIo2/DxeIoLibCpuIo2.inf
      PeCoffLib                   | MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
      PeCoffGetEntryPointLib      | MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
      CacheMaintenanceLib         | MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
      NULL                        | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      SynchronizationLib          | MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
      PciLib                      | MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
      PciExpressLib               | MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
      RegisterFilterLib           | MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
      PeCoffExtraActionLib        | MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
      HobLib                      | MdePkg/Library/DxeHobLib/DxeHobLib.inf
      StackCheckFailureHookLib    | MdePkg/Library/StackCheckFailureHookLibNull/StackCheckFailureHookLibNull.inf
      StackCheckLib               | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      SafeIntLib                  | MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
      UefiDriverEntryPoint        | MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
      UefiBootServicesTableLib    | MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
      MemoryAllocationLib         | MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
      DxeServicesLib              | MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
      DxeServicesTableLib         | MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
      DebugPrintErrorLevelLib     | MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
      AssertLib                   | AdvLoggerPkg/Library/AssertLib/AssertLib.inf
      AdvancedLoggerHdwPortLib    | AdvLoggerPkg/Library/AdvancedLoggerHdwPortLibNull/AdvancedLoggerHdwPortLibNull.inf
      DebugLib                    | AdvLoggerPkg/Library/BaseDebugLibAdvancedLogger/BaseDebugLibAdvancedLogger.inf
      AdvancedLoggerLib           | AdvLoggerPkg/Library/AdvancedLoggerLib/Dxe/AdvancedLoggerLib.inf
      TimerLib                    | PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
  }
  
  OneCryptoPkg/OneCryptoLoaders/OneCryptoLoaderSupvMm.inf {
    <LibraryClasses>
      BaseLib                      | MdePkg/Library/BaseLib/BaseLib.inf
      BaseMemoryLib                | MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
      DebugLib                     | MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      PcdLib                       | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      PrintLib                     | MdePkg/Library/BasePrintLib/BasePrintLib.inf
      UefiLib                      | MdePkg/Library/UefiLib/UefiLib.inf
      UefiRuntimeServicesTableLib  | MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
      DevicePathLib                | MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
      RngLib                       | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # Drivers should use the protocol, GetRandomNumber64 will not work.
      TimerLib                     | MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
      DebugPrintErrorLevelLib      | MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
      SynchronizationLib           | MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
      PciLib                       | MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
      PciExpressLib                | MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
      IoLib                        | MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
      RegisterFilterLib            | MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
      PeCoffExtraActionLib         | MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
      HobLib                       | MdePkg/Library/DxeHobLib/DxeHobLib.inf
      StackCheckFailureHookLib     | MdePkg/Library/StackCheckFailureHookLibNull/StackCheckFailureHookLibNull.inf
      StackCheckLib                | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      SafeIntLib                   | MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
      PeCoffLib                    | MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
      PeCoffGetEntryPointLib       | MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
      CacheMaintenanceLib          | MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
      NULL                         | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      AssertLib                    | AdvLoggerPkg/Library/AssertLib/AssertLib.inf
      AdvancedLoggerHdwPortLib     | AdvLoggerPkg/Library/AdvancedLoggerHdwPortLibNull/AdvancedLoggerHdwPortLibNull.inf
      StandaloneMmDriverEntryPoint | MmSupervisorPkg/Library/StandaloneMmDriverEntryPoint/StandaloneMmDriverEntryPoint.inf
      MmServicesTableLib           | MmSupervisorPkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
      MemoryAllocationLib          | StandaloneMmPkg/Library/StandaloneMmMemoryAllocationLib/StandaloneMmMemoryAllocationLib.inf
      HobLib                       | StandaloneMmPkg/Library/StandaloneMmHobLib/StandaloneMmHobLib.inf
      FvLib                        | StandaloneMmPkg/Library/FvLib/FvLib.inf
  }

  OneCryptoPkg/OneCryptoLoaders/OneCryptoLoaderStandaloneMm.inf {
    <LibraryClasses>
      BaseLib                      | MdePkg/Library/BaseLib/BaseLib.inf
      BaseMemoryLib                | MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
      DebugLib                     | MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      PcdLib                       | MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      PrintLib                     | MdePkg/Library/BasePrintLib/BasePrintLib.inf
      UefiLib                      | MdePkg/Library/UefiLib/UefiLib.inf
      UefiRuntimeServicesTableLib  | MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
      DevicePathLib                | MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
      RngLib                       | MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf # Drivers should use the protocol, GetRandomNumber64 will not work.
      TimerLib                     | MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
      DebugPrintErrorLevelLib      | MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
      SynchronizationLib           | MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
      PciLib                       | MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
      PciExpressLib                | MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
      IoLib                        | MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
      RegisterFilterLib            | MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
      PeCoffExtraActionLib         | MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
      HobLib                       | MdePkg/Library/DxeHobLib/DxeHobLib.inf
      StackCheckFailureHookLib     | MdePkg/Library/StackCheckFailureHookLibNull/StackCheckFailureHookLibNull.inf
      StackCheckLib                | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      SafeIntLib                   | MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
      PeCoffLib                    | MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
      PeCoffGetEntryPointLib       | MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
      CacheMaintenanceLib          | MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
      NULL                         | MdePkg/Library/StackCheckLib/StackCheckLib.inf
      AssertLib                    | AdvLoggerPkg/Library/AssertLib/AssertLib.inf
      AdvancedLoggerHdwPortLib     | AdvLoggerPkg/Library/AdvancedLoggerHdwPortLibNull/AdvancedLoggerHdwPortLibNull.inf
      StandaloneMmDriverEntryPoint | MdePkg/Library/StandaloneMmDriverEntryPoint/StandaloneMmDriverEntryPoint.inf
      MmServicesTableLib           | MdePkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf
      MemoryAllocationLib          | StandaloneMmPkg/Library/StandaloneMmMemoryAllocationLib/StandaloneMmMemoryAllocationLib.inf
      HobLib                       | StandaloneMmPkg/Library/StandaloneMmHobLib/StandaloneMmHobLib.inf
      FvLib                        | StandaloneMmPkg/Library/FvLib/FvLib.inf
  }

  #############################################################################
  ## OneCryptoLoader END
  #############################################################################

[BuildOptions]
  *_*_*_CC_FLAGS = -D DISABLE_NEW_DEPRECATED_INTERFACES
  MSFT:*_*_*_DLINK_FLAGS = /IGNORE:4217
