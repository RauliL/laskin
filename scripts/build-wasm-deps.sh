#!/usr/bin/env bash
set -euo pipefail

GMP_VERSION="${GMP_VERSION:-6.3.0}"
MPFR_VERSION="${MPFR_VERSION:-4.2.2}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
DEPS_DIR="${WASM_DEPS_DIR:-${PROJECT_DIR}/wasm-deps}"
SOURCES_DIR="${DEPS_DIR}/sources"
INSTALL_PREFIX="${DEPS_DIR}/install"

if [[ -z "${EMSCRIPTEN:-}" ]]; then
  if [[ -f "${EMSDK:-/opt/emsdk}/emsdk_env.sh" ]]; then
    # shellcheck source=/dev/null
    source "${EMSDK:-/opt/emsdk}/emsdk_env.sh"
  else
    echo "Emscripten not found. Install the emsdk and source emsdk_env.sh." >&2
    exit 1
  fi
fi

mkdir -p "${SOURCES_DIR}"

build_gmp() {
  local archive="gmp-${GMP_VERSION}.tar.xz"
  local source_dir="${SOURCES_DIR}/gmp-${GMP_VERSION}"

  if [[ ! -d "${source_dir}" ]]; then
    curl -L "https://gmplib.org/download/gmp/${archive}" -o "${SOURCES_DIR}/${archive}"
    tar -xJf "${SOURCES_DIR}/${archive}" -C "${SOURCES_DIR}"
  fi

  pushd "${source_dir}" >/dev/null
  if [[ ! -f Makefile ]]; then
    emconfigure ./configure \
      --disable-assembly \
      --host=none \
      --enable-cxx \
      --prefix="${INSTALL_PREFIX}"
  fi
  emmake make -j"$(nproc)"
  emmake make install
  popd >/dev/null
}

build_mpfr() {
  local archive="mpfr-${MPFR_VERSION}.tar.xz"
  local source_dir="${SOURCES_DIR}/mpfr-${MPFR_VERSION}"

  if [[ ! -d "${source_dir}" ]]; then
    curl -L "https://www.mpfr.org/mpfr-current/${archive}" -o "${SOURCES_DIR}/${archive}"
    tar -xJf "${SOURCES_DIR}/${archive}" -C "${SOURCES_DIR}"
  fi

  pushd "${source_dir}" >/dev/null
  if [[ ! -f Makefile ]]; then
    emconfigure ./configure \
      --host=none \
      --with-gmp="${INSTALL_PREFIX}" \
      --prefix="${INSTALL_PREFIX}"
  fi
  emmake make -j"$(nproc)"
  emmake make install
  popd >/dev/null
}

build_gmp
build_mpfr

echo "WebAssembly dependencies installed to ${INSTALL_PREFIX}"
