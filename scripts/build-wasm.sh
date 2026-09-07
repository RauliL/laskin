#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
DEPS_DIR="${WASM_DEPS_DIR:-${PROJECT_DIR}/wasm-deps}"
INSTALL_PREFIX="${WASM_INSTALL_PREFIX:-${DEPS_DIR}/install}"
BUILD_DIR="${WASM_BUILD_DIR:-${PROJECT_DIR}/build-wasm}"
BUILD_TYPE="${BUILD_TYPE:-Release}"

# Reuse a prebuilt peelo-number WASM prefix when available and ours is missing.
if [[ ! -f "${INSTALL_PREFIX}/lib/libmpfr.a" ]]; then
  PEELO_NUMBER_PREFIX="${PEELO_NUMBER_PREFIX:-${PROJECT_DIR}/../peelo-number/wasm-deps/install}"
  if [[ -f "${PEELO_NUMBER_PREFIX}/lib/libmpfr.a" ]]; then
    INSTALL_PREFIX="${PEELO_NUMBER_PREFIX}"
    echo "Using MPFR/GMP from ${INSTALL_PREFIX}"
  fi
fi

if [[ -z "${EMSCRIPTEN:-}" ]]; then
  if [[ -f "${EMSDK:-/opt/emsdk}/emsdk_env.sh" ]]; then
    # shellcheck source=/dev/null
    source "${EMSDK:-/opt/emsdk}/emsdk_env.sh"
  else
    echo "Emscripten not found. Install the emsdk and source emsdk_env.sh." >&2
    exit 1
  fi
fi

if [[ ! -f "${INSTALL_PREFIX}/lib/libmpfr.a" ]]; then
  echo "MPFR for WebAssembly not found. Building dependencies..."
  "${SCRIPT_DIR}/build-wasm-deps.sh"
  INSTALL_PREFIX="${DEPS_DIR}/install"
fi

emcmake cmake \
  -B "${BUILD_DIR}" \
  -S "${PROJECT_DIR}" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DMPFR_ROOT="${INSTALL_PREFIX}" \
  -DLASKIN_ENABLE_WEB=ON \
  -DLASKIN_ENABLE_CLI=OFF \
  -DLASKIN_ENABLE_GUI=OFF \
  -DLASKIN_ENABLE_2CPP=OFF

emmake cmake --build "${BUILD_DIR}" --parallel "$(nproc)" --target laskin-web

echo "WebAssembly package written to ${PROJECT_DIR}/web/dist"
