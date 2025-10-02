# OneCryptoPkg

OneCryptoPkg provides minimal crypto libraries for standalone MM drivers.

## Directory Structure

```
OneCryptoPkg/
├── OneCryptoPkg.dec              # Package declaration
├── OneCryptoPkg.dsc              # Platform description with library mappings
├── Include/
│   └── Library/
│       ├── MinimalBaseLib.h      # Minimal BaseLib header
│       ├── MinimalBaseMemoryLib.h # Minimal BaseMemoryLib header
│       └── MinimalBasePrintLib.h # Minimal BasePrintLib header
└── Library/
    ├── MinimalBaseLib/           # Minimal BaseLib implementation
    ├── MinimalBaseMemoryLib/     # Minimal BaseMemoryLib implementation
    ├── MinimalBasePrintLib/      # Minimal BasePrintLib implementation
    └── MinimalStandaloneMmDriverEntryPoint/ # Modified entry point without constructors/destructors
```

## Library Mappings

The following library mappings are defined in OneCryptoPkg.dsc:

- `UefiBootServicesTableLib` → `MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf`
- `BaseLib` → `OneCryptoPkg/Library/MinimalBaseLib/MinimalBaseLib.inf` (Minimal BaseLib to satisfy dependencies)
- `BaseMemoryLib` → `OneCryptoPkg/Library/MinimalBaseMemoryLib/MinimalBaseMemoryLib.inf`
- `PrintLib` → `OneCryptoPkg/Library/MinimalBasePrintLib/MinimalBasePrintLib.inf`
- `MmServicesTableLib` → `Features/MM_SUPV/MmSupervisorPkg/Library/StandaloneMmServicesTableLib/StandaloneMmServicesTableLib.inf`
- `StandaloneMmDriverEntryPoint` → `OneCryptoPkg/Library/MinimalStandaloneMmDriverEntryPoint/MinimalStandaloneMmDriverEntryPoint.inf`
- `PcdLib` → `MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf` (NULL implementation)
- `DebugLib` → `MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf` (NULL implementation)

## Key Modifications

### MinimalStandaloneMmDriverEntryPoint

This library is based on the original `Features/MM_SUPV/MmSupervisorPkg/Library/StandaloneMmDriverEntryPoint` but with the following modifications:

**Removed:**
- `ProcessLibraryConstructorList (ImageHandle, MmSystemTable);` - Library constructor calls
- `ProcessLibraryDestructorList (ImageHandle, MmSystemTable);` - Library destructor calls on error

The modified entry point only calls `ProcessModuleEntryPointList()` and returns the status directly.

### Minimal Libraries

The minimal library implementations (`MinimalBaseLib`, `MinimalBaseMemoryLib`, `MinimalBasePrintLib`) are placeholder libraries where functions will be defined as needed. This allows for a controlled, minimal implementation that only includes required functionality.

## Usage

Include this package in your platform DSC file and reference the library mappings as needed for crypto-related standalone MM drivers.