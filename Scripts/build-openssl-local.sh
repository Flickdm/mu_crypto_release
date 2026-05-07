#!/bin/bash
# Build OpenSSL from the repo submodule for local testing.
#
# Produces a standalone OpenSSL CLI binary and libraries under
# Build/openssl-local/ without modifying the submodule working tree.
#
# Usage:
#   ./Scripts/build-openssl-local.sh          # default parallel build
#   ./Scripts/build-openssl-local.sh -j1      # serial build (debugging)
#
# The resulting binary can be used to generate test artifacts:
#   Build/openssl-local/bin/openssl genpkey -algorithm ML-DSA-65 -out key.pem
#   Build/openssl-local/bin/openssl version
#
# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OPENSSL_SRC="$REPO_ROOT/OpensslPkg/Library/OpensslLib/openssl"
BUILD_DIR="$REPO_ROOT/Build/openssl-local"
INSTALL_DIR="$BUILD_DIR/install"

if [ ! -d "$OPENSSL_SRC" ]; then
    echo "ERROR: OpenSSL source not found at $OPENSSL_SRC"
    echo "       Run 'git submodule update --init' first."
    exit 1
fi

# Default to nproc parallel jobs; allow override via args (e.g. -j1)
MAKE_ARGS="${@:--j$(nproc)}"

echo "=== Building OpenSSL from $OPENSSL_SRC ==="
echo "    Build dir:   $BUILD_DIR"
echo "    Install dir: $INSTALL_DIR"
echo "    Make args:   $MAKE_ARGS"
echo ""

# Use an out-of-tree build directory to avoid polluting the submodule
mkdir -p "$BUILD_DIR/build"
cd "$BUILD_DIR/build"

# Configure — no shared libs, no tests, no docs for speed
"$OPENSSL_SRC/Configure" \
    --prefix="$INSTALL_DIR" \
    --openssldir="$INSTALL_DIR/ssl" \
    no-shared \
    no-tests \
    no-docs \
    no-ui-console \
    2>&1 | tail -3

echo ""
echo "=== Compiling (this may take a few minutes) ==="
make $MAKE_ARGS 2>&1 | tail -5

echo ""
echo "=== Installing ==="
make install_sw 2>&1 | tail -3

echo ""
echo "=== Done ==="
"$INSTALL_DIR/bin/openssl" version
echo ""
echo "OpenSSL binary: $INSTALL_DIR/bin/openssl"
echo ""
echo "Quick test commands:"
echo "  $INSTALL_DIR/bin/openssl genpkey -algorithm ML-DSA-87 -out /tmp/ml-dsa-87.pem"
echo "  $INSTALL_DIR/bin/openssl genpkey -algorithm ML-KEM-768 -out /tmp/ml-kem-768.pem"
echo "  $INSTALL_DIR/bin/openssl list -signature-algorithms | grep ML-DSA"
echo "  $INSTALL_DIR/bin/openssl list -kem-algorithms | grep ML-KEM"
