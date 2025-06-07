#!/bin/bash
#
# Simple wrapper for the Chronovyan build system.
# Forwards all arguments to scripts/build.sh.
# This is a convenience wrapper to make building from the project root easier.
#
# Example:
#   ./build.sh --build-type Release --clean
#

set -euo pipefail

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_SCRIPT="$SCRIPT_DIR/scripts/build.sh"

if [ ! -f "$BUILD_SCRIPT" ]; then
    echo "Error: Could not find build script at $BUILD_SCRIPT" >&2
    exit 1
fi

# Forward all arguments to the main build script
exec "$BUILD_SCRIPT" "$@"
