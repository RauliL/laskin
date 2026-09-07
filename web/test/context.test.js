import assert from "node:assert/strict";
import { before, describe, it } from "node:test";

import { createLaskin, LaskinError } from "../dist/index.js";

describe("createLaskin", () => {
  it("loads the module and returns a context", async () => {
    const ctx = await createLaskin();

    assert.equal(typeof ctx.run, "function");
    assert.equal(ctx.depth(), 0);
  });

  it("creates independent contexts", async () => {
    const a = await createLaskin();
    const b = await createLaskin();

    a.run("1 2 +");
    assert.equal(a.depth(), 1);
    assert.equal(a.peek(), "3");
    assert.equal(b.depth(), 0);
  });

  it("honors a custom locateFile", async () => {
    const seen = [];
    const ctx = await createLaskin({
      locateFile(path) {
        seen.push(path);
        return new URL(`../dist/${path}`, import.meta.url).href;
      },
    });

    assert.ok(seen.some((entry) => entry.endsWith(".wasm")));
    ctx.run("4 2 /");
    assert.equal(ctx.peek(), "2");
  });
});

describe("LaskinContext", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createLaskin();
  });

  it("evaluates arithmetic and reports stack state", () => {
    ctx.clear();
    ctx.run("10 3 -");

    assert.equal(ctx.depth(), 1);
    assert.equal(ctx.peek(), "7");
    assert.deepEqual(ctx.stack(), ["7"]);
  });

  it("returns stack values with the top at index 0", () => {
    ctx.clear();
    ctx.run("1 2 3");

    assert.deepEqual(ctx.stack(), ["3", "2", "1"]);
    assert.equal(ctx.peek(), "3");
    assert.equal(ctx.pop(), "3");
    assert.deepEqual(ctx.stack(), ["2", "1"]);
    assert.equal(ctx.depth(), 2);
  });

  it("clears the stack", () => {
    ctx.clear();
    ctx.run("1 2 3");
    ctx.clear();

    assert.equal(ctx.depth(), 0);
    assert.deepEqual(ctx.stack(), []);
  });

  it("captures printed output from .", () => {
    ctx.clear();
    const output = ctx.run('"hello" .');

    assert.equal(output, "hello\n");
    assert.equal(ctx.depth(), 0);
  });

  it("supports measurement units", () => {
    ctx.clear();
    ctx.run("1km 500m +");

    assert.equal(ctx.peek(), "1500m");
  });

  it("supports user-defined words", () => {
    ctx.clear();
    ctx.run("( dup * ) -> square");
    ctx.run("5 square");

    assert.equal(ctx.peek(), "25");
  });

  it("supports vectors", () => {
    ctx.clear();
    ctx.run("[1, 2, 3] [10, 20, 30] +");

    assert.equal(ctx.peek(), "11, 22, 33");
  });
});

describe("LaskinError", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createLaskin();
  });

  it("throws on syntax errors with position metadata", () => {
    ctx.clear();

    assert.throws(
      () => ctx.run("]"),
      (error) => {
        assert.ok(error instanceof LaskinError);
        assert.equal(error.name, "LaskinError");
        assert.equal(error.type, "syntax");
        assert.equal(error.line, 1);
        assert.equal(error.column, 0);
        assert.match(error.message, /syntax/i);
        return true;
      },
    );
  });

  it("throws on name errors", () => {
    ctx.clear();

    assert.throws(
      () => ctx.run("no-such-word"),
      (error) => {
        assert.ok(error instanceof LaskinError);
        assert.equal(error.type, "name");
        return true;
      },
    );
  });

  it("throws on type errors", () => {
    ctx.clear();

    assert.throws(
      () => ctx.run("true 1 +"),
      (error) => {
        assert.ok(error instanceof LaskinError);
        assert.equal(error.type, "type");
        return true;
      },
    );
  });

  it("throws on range errors when peeking an empty stack", () => {
    ctx.clear();

    assert.throws(
      () => ctx.peek(),
      (error) => {
        assert.ok(error instanceof LaskinError);
        assert.equal(error.type, "range");
        return true;
      },
    );
  });

  it("throws on range errors when popping an empty stack", () => {
    ctx.clear();

    assert.throws(
      () => ctx.pop(),
      (error) => {
        assert.ok(error instanceof LaskinError);
        assert.equal(error.type, "range");
        return true;
      },
    );
  });
});
