#!/usr/bin/env node

import { createLaskin, LaskinError } from './dist/index.js';

const ctx = await createLaskin();

ctx.run('1 2 +');
if (ctx.peek() !== '3') {
  throw new Error(`expected peek() === "3", got ${JSON.stringify(ctx.peek())}`);
}
if (ctx.depth() !== 1) {
  throw new Error(`expected depth() === 1, got ${ctx.depth()}`);
}
if (JSON.stringify(ctx.stack()) !== JSON.stringify(['3'])) {
  throw new Error(`unexpected stack: ${JSON.stringify(ctx.stack())}`);
}

const output = ctx.run('"hello" .');
if (output !== 'hello\n') {
  throw new Error(`unexpected print output: ${JSON.stringify(output)}`);
}

ctx.clear();
if (ctx.depth() !== 0) {
  throw new Error(`expected empty stack after clear(), got depth ${ctx.depth()}`);
}

let caught = false;
try {
  ctx.run(']');
} catch (error) {
  caught = true;
  if (!(error instanceof LaskinError)) {
    throw new Error(`expected LaskinError, got ${error}`);
  }
  if (error.type !== 'syntax') {
    throw new Error(`expected syntax error, got ${error.type}: ${error.message}`);
  }
}
if (!caught) {
  throw new Error('expected syntax error from "]"');
}

console.log('web smoke test ok');
