# Laskin (WebAssembly)

JavaScript/TypeScript bindings for [Laskin], compiled to WebAssembly with
[Emscripten].

This package is **not published yet**. Build it locally with:

```bash
./scripts/build-wasm.sh
```

Artifacts are written to `web/dist/` (`index.js`, `laskin.js`, `laskin.wasm`, …).

## Usage

```js
import { createLaskin, LaskinError } from "./dist/index.js";

const ctx = await createLaskin();
ctx.run("1 2 +");
console.log(ctx.peek()); // "3"

try {
  ctx.run("1 0 /");
} catch (error) {
  if (error instanceof LaskinError) {
    console.error(error.type, error.message);
  }
}
```

## API

- `createLaskin(options?)` — load the WASM module and return a context
- `context.run(source)` — evaluate source; returns captured output (e.g. from `.`)
- `context.clear()` / `depth()` / `peek()` / `pop()` / `stack()`
- `LaskinError` — thrown on interpreter errors (`type`, optional `line`/`column`)

File includes (`include`) are disabled in this build.

[Laskin]: https://github.com/RauliL/laskin
[Emscripten]: https://emscripten.org/
