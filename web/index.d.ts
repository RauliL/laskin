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

/** Options for loading the WebAssembly module. */
export interface CreateLaskinOptions {
  /**
   * Resolve the path to `laskin.wasm` (and related assets).
   * Defaults to a URL next to this package's `index.js`.
   */
  locateFile?: (path: string, prefix: string) => string;
}

/** A Laskin interpreter context. */
export interface LaskinContext {
  /**
   * Evaluate Laskin source code.
   * @returns Captured interpreter output (e.g. from `.`).
   */
  run(source: string): string;

  /** Clear the data stack. */
  clear(): void;

  /** Number of values currently on the stack. */
  depth(): number;

  /** String representation of the top-of-stack value. */
  peek(): string;

  /** Pop and return the string representation of the top-of-stack value. */
  pop(): string;

  /**
   * String representations of all stack values.
   * Index 0 is the top of the stack.
   */
  stack(): string[];
}

/** @internal Native Embind context (not part of the public API). */
export interface LaskinContextNative {
  run(source: string): string;
  clear(): void;
  depth(): number;
  peek(): string;
  pop(): string;
  stack(): string[];
}

/**
 * Initialize the WebAssembly module and create a new interpreter context.
 */
export function createLaskin(
  options?: CreateLaskinOptions,
): Promise<LaskinContext>;

export default createLaskin;
