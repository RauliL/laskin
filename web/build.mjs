#!/usr/bin/env zx

import { $, argv, cd, echo, fs, os, path, useBash, within } from "zx";
import { fileURLToPath } from "node:url";

useBash();

const GMP_VERSION = process.env.GMP_VERSION ?? "6.3.0";
const MPFR_VERSION = process.env.MPFR_VERSION ?? "4.2.2";

const scriptDir = path.dirname(fileURLToPath(import.meta.url));
const depsDir = path.join(scriptDir, "deps");
const sourcesDir = path.join(depsDir, "sources");
let installPrefix = path.join(depsDir, "install");
const buildDir = path.join(scriptDir, "build");
const buildType = process.env.BUILD_TYPE ?? "Release";
const jobs = os.cpus().length;

function ensureEmscripten() {
  if (process.env.EMSCRIPTEN) return;

  const emsdk = process.env.EMSDK ?? "/opt/emsdk";
  const envSh = path.join(emsdk, "emsdk_env.sh");
  if (!fs.existsSync(envSh)) {
    console.error(
      "Emscripten not found. Install the emsdk and source emsdk_env.sh.",
    );
    process.exit(1);
  }

  $.prefix += `source ${envSh}; `;
}

async function downloadAndExtract(url, archive, sourceDir) {
  const archivePath = path.join(sourcesDir, archive);

  if (!fs.existsSync(sourceDir)) {
    const response = await fetch(url);
    if (!response.ok) {
      throw new Error(`Failed to download ${url}: ${response.status}`);
    }
    await fs.writeFile(archivePath, Buffer.from(await response.arrayBuffer()));
    await $`tar -xJf ${archivePath} -C ${sourcesDir}`;
  }
}

async function buildGmp() {
  const archive = `gmp-${GMP_VERSION}.tar.xz`;
  const sourceDir = path.join(sourcesDir, `gmp-${GMP_VERSION}`);

  await downloadAndExtract(
    `https://ftp.gnu.org/gnu/gmp/${archive}`,
    archive,
    sourceDir,
  );

  await within(async () => {
    cd(sourceDir);
    if (!fs.existsSync("Makefile")) {
      await $`emconfigure ./configure --disable-assembly --host=none --enable-cxx --prefix=${installPrefix}`;
    }
    await $`emmake make -j${jobs}`;
    await $`emmake make install`;
  });
}

async function buildMpfr() {
  const archive = `mpfr-${MPFR_VERSION}.tar.xz`;
  const sourceDir = path.join(sourcesDir, `mpfr-${MPFR_VERSION}`);

  await downloadAndExtract(
    `https://ftp.gnu.org/gnu/mpfr/${archive}`,
    archive,
    sourceDir,
  );

  await within(async () => {
    cd(sourceDir);
    if (!fs.existsSync("Makefile")) {
      await $`emconfigure ./configure --host=none --with-gmp=${installPrefix} --prefix=${installPrefix}`;
    }
    await $`emmake make -j${jobs}`;
    await $`emmake make install`;
  });
}

async function buildDeps() {
  await fs.ensureDir(sourcesDir);
  await buildGmp();
  await buildMpfr();
  echo(`WebAssembly dependencies installed to ${installPrefix}`);
}

if (argv["deps-only"]) {
  ensureEmscripten();
  installPrefix = path.join(depsDir, "install");
  await buildDeps();
  process.exit(0);
}

ensureEmscripten();

if (!fs.existsSync(path.join(installPrefix, "lib/libmpfr.a"))) {
  echo("MPFR for WebAssembly not found. Building dependencies...");
  installPrefix = path.join(depsDir, "install");
  await buildDeps();
}

await $`emcmake cmake -B ${buildDir} -S ${path.join(scriptDir, "..")} -DCMAKE_BUILD_TYPE=${buildType} -DMPFR_ROOT=${installPrefix} -DLASKIN_ENABLE_WEB=ON -DLASKIN_ENABLE_CLI=OFF -DLASKIN_ENABLE_GUI=OFF -DLASKIN_ENABLE_2CPP=OFF`;

await $`emmake cmake --build ${buildDir} --parallel ${jobs} --target laskin-web`;

echo(`WebAssembly build written to ${scriptDir}`);
