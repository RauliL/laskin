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
export type CreateContextOptions = {
  /**
   * Resolve the path to `laskin.wasm` (and related assets).
   * Defaults to a URL next to this package's `index.js`.
   */
  locateFile?: (path: string, prefix: string) => string;
};

/** Supported Laskin value type names. */
export type LaskinValueType =
  | "boolean"
  | "number"
  | "string"
  | "vector"
  | "record"
  | "quote"
  | "date"
  | "time"
  | "month"
  | "weekday";

export type LaskinBoolean = {
  type: "boolean";
  value: boolean;
};

export type LaskinNumber = {
  type: "number";
  /** Numeric with optional unit, e.g. `"1500m"`. */
  value: string;
};

export type LaskinString = {
  type: "string";
  value: string;
};

export type LaskinVector = {
  type: "vector";
  value: LaskinValue[];
};

export type LaskinRecord = {
  type: "record";
  value: Record<string, LaskinValue>;
};

export type LaskinQuote = {
  type: "quote";
  /** Laskin source for the quote. */
  value: string;
};

export type LaskinDate = {
  type: "date";
  /** ISO-like date string, e.g. `"2026-09-07"`. */
  value: string;
};

export type LaskinTime = {
  type: "time";
  /** Time string, e.g. `"16:02:00"`. */
  value: string;
};

export type LaskinMonth = {
  type: "month";
  /** Full month name, e.g. `"january"`. */
  value:
    | "january"
    | "february"
    | "march"
    | "april"
    | "may"
    | "june"
    | "july"
    | "august"
    | "september"
    | "october"
    | "november"
    | "december";
};

export type LaskinWeekday = {
  type: "weekday";
  /** Full weekday name, e.g. `"monday"`. */
  value:
    | "sunday"
    | "monday"
    | "tuesday"
    | "wednesday"
    | "thursday"
    | "friday"
    | "saturday";
};

/**
 * A value on the Laskin data stack.
 */
export type LaskinValue =
  | LaskinBoolean
  | LaskinNumber
  | LaskinString
  | LaskinVector
  | LaskinRecord
  | LaskinQuote
  | LaskinDate
  | LaskinTime
  | LaskinMonth
  | LaskinWeekday;

/**
 * Format a Laskin value as a human-readable string.
 *
 * Uses the same formatting as the interpreter's `>string` word.
 */
export function laskinValueToString(
  value: LaskinValue,
  options?: CreateContextOptions,
): Promise<string>;

/**
 * Format a Laskin value as source code that can be evaluated again.
 *
 * Uses the same formatting as the interpreter's `>source` word.
 */
export function laskinValueToSource(
  value: LaskinValue,
  options?: CreateContextOptions,
): Promise<string>;

/**
 * A Laskin interpreter context.
 */
export type LaskinContext = {
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
  peek(): LaskinValue;

  /**
   * Removes and returns the top-of-stack value or throws exception if the
   * stack is empty.
   */
  pop(): LaskinValue;

  /**
   * Pushes a value onto the data stack.
   */
  push(value: LaskinValue): void;

  /**
   * Returns all stack values as an array.
   */
  stack(): LaskinValue[];

  /**
   * Returns copy of context dictionary.
   */
  dictionary(): Record<string, LaskinValue>;
};

/**
 * Initialize the WebAssembly module and create a new interpreter context.
 */
export function createContext(
  options?: CreateContextOptions,
): Promise<LaskinContext>;
