#!/usr/bin/env bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SDK_DIR="${SCRIPT_DIR}/../sdk"
OUT_DIR="${SCRIPT_DIR}/../out"
ARGS_GN="${OUT_DIR}/args.gn"

mkdir -p "${OUT_DIR}"
rm -f "${ARGS_GN}"

echo "toolchain_path = \"${SDK_DIR}/toolchain\"" > "${ARGS_GN}"
echo "sysroot_path = \"${SDK_DIR}/sysroot\"" >> "${ARGS_GN}"
echo "target_triple = \"arm-linux-gnueabihf\"" >> "${ARGS_GN}"
echo "extra_system_include_dirs = [\"/opt/vc/include\"]" >> "${ARGS_GN}"
echo "extra_system_lib_dirs = [\"/opt/vc/lib\"]" >> "${ARGS_GN}"

"${SDK_DIR}/gn" gen "${OUT_DIR}"
