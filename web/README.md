# Laskin

[![npm](https://img.shields.io/npm/v/laskin.svg)](https://www.npmjs.com/package/laskin)
[![build](https://github.com/RauliL/laskin/actions/workflows/build.yml/badge.svg)](https://github.com/RauliL/laskin/actions/workflows/build.yml)

WebAssembly build of [Laskin] — a reverse Polish notation calculator /
programming language inspired by Forth, RPL and Plorth.

## Installation

```bash
npm install laskin
```

Requires Node.js 22+ (or a modern browser with WebAssembly exception support).

## Usage

```js
import { createContext, LaskinError } from "laskin";

const ctx = await createContext();
ctx.run("1 2 +");
console.log(ctx.peek()); // "3"

const output = ctx.run('"hello" .');
console.log(output); // "hello\n"

try {
  ctx.run("]");
} catch (error) {
  if (error instanceof LaskinError) {
    console.error(error.type, error.message, error.line, error.column);
  }
}
```

## API

- `createContext(options?)` — load the WASM module and return a context
- `context.run(source)` — evaluate source; returns captured output (e.g. from `.`)
- `context.clear()` / `depth()` / `peek()` / `pop()` / `stack()`
- `LaskinError` — thrown on interpreter errors (`type`, optional `line`/`column`)

File includes (`include`) are disabled in this build.

### Options

```ts
await createContext({
  locateFile(path) {
    // Resolve laskin.wasm for bundlers / custom asset paths
    return `/assets/${path}`;
  },
});
```

## Building from source

Requires the [Emscripten SDK].

```bash
./scripts/build-wasm.sh
npm test --prefix web
```

[Laskin]: https://github.com/RauliL/laskin
[Emscripten SDK]: https://emscripten.org/docs/getting-started/downloads.html
