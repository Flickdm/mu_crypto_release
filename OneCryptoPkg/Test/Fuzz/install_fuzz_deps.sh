#!/bin/bash
# install_fuzz_deps.sh
#
# Install dependencies required for OneCrypto fuzzing on Ubuntu/Debian systems.
#
# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

set -e

echo "=========================================="
echo "OneCrypto Fuzzing Dependencies Installer"
echo "=========================================="
echo ""

# Detect package manager
if command -v apt-get &> /dev/null; then
    PKG_MANAGER="apt"
elif command -v dnf &> /dev/null; then
    PKG_MANAGER="dnf"
elif command -v yum &> /dev/null; then
    PKG_MANAGER="yum"
else
    echo "ERROR: Unsupported package manager. Please install dependencies manually."
    exit 1
fi

echo "Detected package manager: $PKG_MANAGER"
echo ""

# Function to install packages on Debian/Ubuntu
install_apt() {
    echo "Installing packages via apt..."
    sudo apt-get update
    
    # Core build tools
    sudo apt-get install -y \
        build-essential \
        clang \
        llvm
    
    # Clang runtime library (includes libFuzzer and sanitizers)
    # Try to detect the clang version and install matching runtime
    CLANG_VERSION=$(clang --version | grep -oP 'clang version \K[0-9]+' | head -1)
    if [ -n "$CLANG_VERSION" ]; then
        echo "Detected Clang version: $CLANG_VERSION"
        sudo apt-get install -y "libclang-rt-${CLANG_VERSION}-dev" || \
            sudo apt-get install -y libclang-rt-dev
    else
        sudo apt-get install -y libclang-rt-dev
    fi
    
    echo ""
    echo "APT packages installed successfully."
}

# Function to install packages on Fedora/RHEL
install_dnf() {
    echo "Installing packages via dnf..."
    sudo dnf install -y \
        clang \
        clang-analyzer \
        compiler-rt \
        llvm
    
    echo ""
    echo "DNF packages installed successfully."
}

# Function to install packages on older RHEL/CentOS
install_yum() {
    echo "Installing packages via yum..."
    sudo yum install -y \
        clang \
        compiler-rt \
        llvm
    
    echo ""
    echo "YUM packages installed successfully."
}

# Install based on package manager
case $PKG_MANAGER in
    apt)
        install_apt
        ;;
    dnf)
        install_dnf
        ;;
    yum)
        install_yum
        ;;
esac

# Verify installation
echo ""
echo "Verifying installation..."
echo ""

# Check clang
if command -v clang &> /dev/null; then
    echo "✓ clang: $(clang --version | head -1)"
else
    echo "✗ clang not found"
    exit 1
fi

# Check for libFuzzer
echo -n "Checking libFuzzer... "
FUZZER_TEST=$(mktemp)
cat > "${FUZZER_TEST}.c" << 'EOF'
int LLVMFuzzerTestOneInput(const char *data, long size) { return 0; }
EOF

if clang -fsanitize=fuzzer "${FUZZER_TEST}.c" -o "$FUZZER_TEST" 2>/dev/null; then
    echo "✓ libFuzzer available"
    rm -f "$FUZZER_TEST" "${FUZZER_TEST}.c"
else
    echo "✗ libFuzzer not available"
    rm -f "${FUZZER_TEST}.c"
    echo ""
    echo "WARNING: libFuzzer is not available. You can still use the standalone test driver."
fi

# Check for AddressSanitizer
echo -n "Checking AddressSanitizer... "
ASAN_TEST=$(mktemp)
cat > "${ASAN_TEST}.c" << 'EOF'
int main() { return 0; }
EOF

if clang -fsanitize=address "${ASAN_TEST}.c" -o "$ASAN_TEST" 2>/dev/null; then
    echo "✓ AddressSanitizer available"
    rm -f "$ASAN_TEST" "${ASAN_TEST}.c"
else
    echo "✗ AddressSanitizer not available"
    rm -f "${ASAN_TEST}.c"
fi

echo ""
echo "=========================================="
echo "Installation complete!"
echo "=========================================="
echo ""
echo "Next steps:"
echo "  1. Build the OneCrypto binary:"
echo "     cd /path/to/ONE_CRYPTO"
echo "     stuart_build -c OneCryptoPkg/PlatformBuild.py"
echo ""
echo "  2. Build the fuzzer:"
echo "     cd OneCryptoPkg/Test/Fuzz"
echo "     make fuzzer"
echo ""
echo "  3. Run the fuzzer:"
echo "     export ONECRYPTO_BIN=/path/to/OneCryptoBinSupvMm.efi"
echo "     ./build/FuzzSha256Host corpus/sha256/"
echo ""
