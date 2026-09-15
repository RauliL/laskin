import type { Value } from "./value.d.ts";

/**
 * JSON-compatible snapshot of a Laskin context.
 *
 * `stack` is ordered with the top of the stack at index 0. `dictionary`
 * contains only non-native words.
 */
export type ContextSnapshot = {
  stack: Value[];
  dictionary: Record<string, Value>;
};
