# Frequently Asked Questions

## Can I back OneCryptoPkg with something other than Openssl?

Yes - As long as the crypto provider implements BaseCryptLib and its
dependencies are met by the `*OnOneCrypto` Libs then its as simple as
updating the DSC.

## Where can I see OneCryptoPkg integrated into a platform?

QemuQ35Pkg and QemuArmVirtPkg on
[mu_tiano_platforms](https://github.com/microsoft/mu_tiano_platforms) use the
OneCrypto binary drivers. See pull request
[Platforms: Wire up OneCrypto binary drivers](https://github.com/microsoft/mu_tiano_platforms/pull/1278).

## OneCryptoBinSupvMm is MODULE_TYPE MM_STANDALONE — how does it run in DXE?

On X64, the DXE Loader (`OneCryptoLoaderDxe`) calls `LoadImage()` on the
`MM_STANDALONE` binary to get the correct memory protections and mappings
applied, then parses the PE/COFF exports to find and invoke the `CryptoEntry`
function.

On AARCH64 there are now two approaches:

1. Legacy dual-bin mode: a dedicated `OneCryptoBinDxe` (`DXE_DRIVER`) is used
  because the secure-world FV is not directly accessible from DXE.
2. Single-copy AARCH64 mode: DXE uses
  `OneCryptoLoaderDxeFromMm` to fetch OneCrypto
  image bytes from `OneCryptoImageProviderStandaloneMm` over
  `EFI_MM_COMMUNICATION2_PROTOCOL`, then calls `LoadImage()`.

See [Architecture.md](Architecture.md) for the full Bin + Loader pattern and
the differences between X64 and AARCH64.

## Why are there both SetupEntry and NoSetupEntry in the Crypto Bin?

When a Bin module is loaded by the environment it was built for (e.g.
`OneCryptoBinDxe` dispatched by the DXE dispatcher, or `OneCryptoBinStandaloneMm`
dispatched by the MM environment), the UEFI loader natively calls all library
constructors. In this case the Loader invokes `NoSetupEntry` — no additional
setup is needed.

When a Loader loads a Bin from a **different phase** (e.g. `OneCryptoLoaderDxe`
on X64 loading the `MM_STANDALONE` binary via `LoadImage()`), the library
constructors are not automatically run because the binary's module type does
not match the executing environment. The Loader must call `SetupEntry` instead,
which manually initializes the library constructors before providing the crypto
protocol.

## Why are there multiple DXE Loaders (OneCryptoLoaderDxe, OneCryptoLoaderDxeByProtocol, OneCryptoLoaderDxeFromMm)?

They serve different architectures with fundamentally different loading
strategies:

- **`OneCryptoLoaderDxe`** (X64) — Locates the `MM_STANDALONE` Bin image in the
  firmware volume via `GetSectionFromAnyFv()`, calls `LoadImage()` to load it
  into DXE memory, and parses the PE/COFF export directory to resolve
  `CryptoEntry`. Because this is a cross-phase load, it uses `SetupEntry` to
  manually run library constructors.

- **`OneCryptoLoaderDxeByProtocol`** (AARCH64) — The Bin is a native
  `DXE_DRIVER` (`OneCryptoBinDxe`) already dispatched by the DXE dispatcher.
  The Loader simply calls `LocateProtocol()` on `gOneCryptoPrivateProtocolGuid`
  to find it. No `LoadImage()` or PE/COFF parsing is needed, and it uses
  `NoSetupEntry` since constructors already ran during normal dispatch.

- **`OneCryptoLoaderDxeFromMm`** (AARCH64 single-copy mode) — DXE cannot read the
  secure-world FV directly, so this loader requests OneCrypto image bytes from
  StandaloneMM (`OneCryptoImageProviderStandaloneMm`) via
  `EFI_MM_COMMUNICATION2_PROTOCOL`, then `LoadImage()`s the resulting PE32 and
  resolves `CryptoEntry`.

See [Architecture.md](Architecture.md) for the full X64 vs AARCH64 breakdown.
