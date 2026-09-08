import assert from "node:assert/strict";
import { before, describe, it } from "node:test";

import {
  createContext,
  laskinValueToSource,
  laskinValueToString,
  LaskinError,
} from "../index.js";

/** @param {string} value */
function number(value) {
  return { type: "number", value };
}

describe("createContext", () => {
  it("loads the module and returns a context", async () => {
    const ctx = await createContext();

    assert.equal(typeof ctx.run, "function");
    assert.equal(ctx.depth(), 0);
  });

  it("creates independent contexts", async () => {
    const a = await createContext();
    const b = await createContext();

    a.run("1 2 +");
    assert.equal(a.depth(), 1);
    assert.deepEqual(a.peek(), number("3"));
    assert.equal(b.depth(), 0);
  });

  it("honors a custom locateFile", async () => {
    const seen = [];
    const ctx = await createContext({
      locateFile(path) {
        seen.push(path);
        return new URL(`../${path}`, import.meta.url).href;
      },
    });

    assert.ok(seen.some((entry) => entry.endsWith(".wasm")));
    ctx.run("4 2 /");
    assert.deepEqual(ctx.peek(), number("2"));
  });
});

describe("LaskinContext", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createContext();
  });

  it("evaluates arithmetic and reports stack state", () => {
    ctx.clear();
    ctx.run("10 3 -");

    assert.equal(ctx.depth(), 1);
    assert.deepEqual(ctx.peek(), number("7"));
    assert.deepEqual(ctx.stack(), [number("7")]);
  });

  it("returns stack values with the top at index 0", () => {
    ctx.clear();
    ctx.run("1 2 3");

    assert.deepEqual(ctx.stack(), [number("3"), number("2"), number("1")]);
    assert.deepEqual(ctx.peek(), number("3"));
    assert.deepEqual(ctx.pop(), number("3"));
    assert.deepEqual(ctx.stack(), [number("2"), number("1")]);
    assert.equal(ctx.depth(), 2);
  });

  it("pushes values that can be used by the interpreter", () => {
    ctx.clear();
    ctx.push(number("10"));
    ctx.push(number("3"));
    ctx.run("-");

    assert.deepEqual(ctx.peek(), number("7"));
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

    assert.deepEqual(ctx.peek(), number("1.5km"));
  });

  it("supports user-defined words", () => {
    ctx.clear();
    ctx.run("( dup * ) -> square");
    ctx.run("5 square");

    assert.deepEqual(ctx.peek(), number("25"));
  });

  it("supports vectors", () => {
    ctx.clear();
    ctx.run("[1, 2, 3] [10, 20, 30] +");

    assert.deepEqual(ctx.peek(), {
      type: "vector",
      value: [number("11"), number("22"), number("33")],
    });
  });

  it("supports booleans", () => {
    ctx.clear();
    ctx.run("true false and");

    assert.deepEqual(ctx.peek(), { type: "boolean", value: false });
  });

  it("supports strings", () => {
    ctx.clear();
    ctx.run('"hello"');

    assert.deepEqual(ctx.peek(), { type: "string", value: "hello" });
  });

  it("supports records", () => {
    ctx.clear();
    ctx.run('{ "name": "Ada", "age": 36 }');

    assert.deepEqual(ctx.peek(), {
      type: "record",
      value: {
        name: { type: "string", value: "Ada" },
        age: number("36"),
      },
    });
  });
});

describe("LaskinContext.push", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createContext();
  });

  /** @param {string} source */
  function sample(source) {
    ctx.clear();
    ctx.run(source);
    return ctx.peek();
  }

  /** @param {import('../index.d.ts').LaskinValue} value */
  function assertPushPreserves(value) {
    ctx.clear();
    ctx.push(value);
    assert.deepEqual(ctx.peek(), value);
  }

  it("places pushed values on top of the stack in order", () => {
    ctx.clear();
    ctx.push(number("1"));
    ctx.push(number("2"));
    ctx.push({ type: "boolean", value: true });

    assert.equal(ctx.depth(), 3);
    assert.deepEqual(ctx.stack(), [
      { type: "boolean", value: true },
      number("2"),
      number("1"),
    ]);
  });

  it("pushes boolean values", () => {
    assertPushPreserves(sample("true"));
    assertPushPreserves(sample("false"));
  });

  it("pushes number values", () => {
    assertPushPreserves(sample("42"));
    assertPushPreserves(sample("1.5km"));
  });

  it("pushes string values", () => {
    assertPushPreserves(sample('"hello"'));
    assertPushPreserves(sample('"line\\nbreak"'));
  });

  it("pushes vector values", () => {
    assertPushPreserves(sample("[1, 2, 3]"));
    assertPushPreserves(sample('["a", "b"]'));
  });

  it("pushes record values", () => {
    assertPushPreserves(sample('{ "name": "Ada", "age": 36 }'));
  });

  it("pushes quote values", () => {
    for (const source of ["( dup )", "( 1 + )"]) {
      const value = sample(source);
      ctx.clear();
      ctx.push(value);
      const pushed = ctx.peek();

      assert.equal(pushed.type, "quote");
      assert.equal(typeof pushed.value, "string");
      assert.ok(pushed.value.includes("("));
      assert.ok(pushed.value.includes(")"));
    }
  });

  it("pushes date values", () => {
    assertPushPreserves(sample("2026-09-07"));
  });

  it("pushes time values", () => {
    assertPushPreserves(sample("16:02:00"));
  });

  it("pushes month values", () => {
    assertPushPreserves(sample("january"));
    assertPushPreserves(sample("december"));
  });

  it("pushes weekday values", () => {
    assertPushPreserves(sample("monday"));
    assertPushPreserves(sample("sunday"));
  });
});

describe("LaskinContext.dictionary", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createContext();
  });

  it("should provide readonly access to the context dictionary", () => {
    ctx.run("'foo' -> variable");

    const dictionary = ctx.dictionary();

    assert.deepEqual(dictionary.variable, { type: "string", value: "foo" });
  });
});

describe("laskinValueToString", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createContext();
  });

  it("formats values with >string", async () => {
    ctx.clear();
    ctx.run("1km 500m +");
    assert.equal(await laskinValueToString(ctx.peek()), "1.5km");

    ctx.clear();
    ctx.run("[1, 2, 3] [10, 20, 30] +");
    assert.equal(await laskinValueToString(ctx.peek()), "11, 22, 33");

    ctx.clear();
    ctx.run("true false and");
    assert.equal(await laskinValueToString(ctx.peek()), "false");
  });
});

describe("laskinValueToSource", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createContext();
  });

  it("formats values as re-evaluable source code", async () => {
    ctx.clear();
    ctx.run("1km 500m +");
    assert.equal(await laskinValueToSource(ctx.peek()), "1.5km");

    ctx.clear();
    ctx.run('"hello"');
    assert.equal(await laskinValueToSource(ctx.peek()), '"hello"');

    ctx.clear();
    ctx.run("[1, 2, 3]");
    assert.equal(await laskinValueToSource(ctx.peek()), "[1, 2, 3]");

    ctx.clear();
    ctx.run('{ "name": "Ada", "age": 36 }');
    assert.equal(
      await laskinValueToSource(ctx.peek()),
      '{"name": "Ada", "age": 36}',
    );
  });

  it("produces source that evaluates to an equal value", async () => {
    ctx.clear();
    ctx.run('{ "name": "Ada", "age": 36 }');

    const value = ctx.peek();
    const source = await laskinValueToSource(value);

    ctx.clear();
    ctx.run(source);
    assert.deepEqual(ctx.peek(), value);
  });
});

describe("LaskinError", () => {
  /** @type {import('../index.d.ts').LaskinContext} */
  let ctx;

  before(async () => {
    ctx = await createContext();
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
