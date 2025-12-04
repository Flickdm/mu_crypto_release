# EDK II Crypto

This repository currently hosts forward-looking development of crypto code for use in the EDK II project. It is
currently under active development as a separate repsository to facilitate easier collaboration and code review for
crypto changes.

Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file for details on contributing to this repository.

If you are interested in using crypto in a production UEFI platform, please see
[CryptoPkg](https://github.com/tianocore/edk2/tree/master/CryptoPkg) in the main EDK II repository.

## Building

To build the code in this repository, you will need to set up an EDK II build environment. Please refer to
[How to Build EDK II With Stuart](https://github.com/tianocore/tianocore.github.io/wiki/How-to-Build-With-Stuart)
and use the following commmands specific to this repository:

These instructions have only been tested on Windows with Visual Studio 2022. It is assumed developers currently
working in the repo are familiar with workspace setup in their preferred environment.

- Setup: `stuart_ci_setup -c ./.pytool/CISettings.py -p CryptoPkg`
- Update: `stuart_update -c ./.pytool/CISettings.py -p CryptoPkg`
- Submodule Update: `git submodule update --init`
- One-Time Tools Build: `python .\EDK2\BaseTools\Edk2ToolsBuild.py -t VS2022`
- Build: `stuart_ci_build -c ./.pytool/CISettings.py -p CryptoPkg`
