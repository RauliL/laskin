/** Supported Laskin error type names. */
export type LaskinErrorType =
  "syntax" | "type" | "unit" | "range" | "domain" | "name" | "system" | "exit";

/** Error thrown by the Laskin interpreter. */
export class LaskinError extends Error {
  readonly name: "LaskinError";
  readonly type: LaskinErrorType | string;
  readonly line?: number;
  readonly column?: number;

  constructor(
    message: string,
    type: LaskinErrorType | string,
    line?: number,
    column?: number,
  );
}

/**
 * Options for loading the WebAssembly module.
 */
export type ModuleOptions = {
  /**
   * Resolve the path to `laskin.wasm` (and related assets).
   * Defaults to a URL next to this package's `index.js`.
   */
  locateFile?: (path: string, prefix: string) => string;
};

export type {
  DefinitionNode,
  LiteralNode,
  Node,
  NodeType,
  Position,
  RecordLiteralNode,
  SymbolNode,
  VectorLiteralNode,
} from "./ast.d.ts";

export type {
  BooleanValue,
  DateValue,
  Month,
  MonthValue,
  NumberValue,
  QuoteValue,
  RecordValue,
  StringValue,
  TimeValue,
  Value,
  ValueType,
  VectorValue,
  Weekday,
  WeekdayValue,
} from "./value.d.ts";

import type { Value } from "./value.d.ts";

/**
 * Format a Laskin value as a human-readable string.
 *
 * Uses the same formatting as the interpreter's `>string` word.
 */
export function valueToString(
  value: Value,
  options?: ModuleOptions,
): Promise<string>;

/**
 * Format a Laskin value as source code that can be evaluated again.
 *
 * Uses the same formatting as the interpreter's `>source` word.
 */
export function valueToSource(
  value: Value,
  options?: ModuleOptions,
): Promise<string>;

/**
 * A Laskin interpreter context.
 */
export type Context = {
  /**
   * Evaluate Laskin source code.
   *
   * @returns Captured interpreter output (e.g. from `.`).
   */
  run(source: string): string;

  /**
   * Clear the data stack.
   */
  clear(): void;

  /**
   * Number of values currently on the stack.
   */
  depth(): number;

  /**
   * Returns top-of-stack value or throws exception if the stack is empty.
   */
  peek(): Value;

  /**
   * Removes and returns the top-of-stack value or throws exception if the
   * stack is empty.
   */
  pop(): Value;

  /**
   * Pushes a value onto the data stack.
   */
  push(value: Value): void;

  /**
   * Returns all stack values as an array.
   */
  stack(): Value[];

  /**
   * Returns copy of context dictionary.
   */
  dictionary(): Record<string, Value>;
};

/**
 * Initialize the WebAssembly module and create a new interpreter context.
 */
export function createContext(options?: ModuleOptions): Promise<Context>;
