import createLaskinModule from "./laskin.js";

/**
 * Error thrown by the Laskin interpreter.
 */
export class LaskinError extends Error {
  constructor(message, type, line, column) {
    super(message);
    this.name = "LaskinError";
    this.type = type;
    if (line != null) {
      this.line = line;
    }
    if (column != null) {
      this.column = column;
    }
  }
}

const rethrow = (error) => {
  if (error && typeof error === "object" && error.name === "LaskinError") {
    throw new LaskinError(error.message, error.type, error.line, error.column);
  }

  throw error;
};

let defaultModulePromise;

const loadModule = (options = {}) => {
  const locateFile =
    typeof options.locateFile === "function"
      ? options.locateFile
      : (path) => new URL(path, import.meta.url).href;

  if (typeof options.locateFile !== "function") {
    defaultModulePromise ??= createLaskinModule({ locateFile });

    return defaultModulePromise;
  }

  return createLaskinModule({ locateFile });
};

/**
 * Initialize the WebAssembly module and create a new interpreter context.
 */
export const createContext = async (options = {}) => {
  const { Context } = await loadModule(options);
  const context = new Context();

  return {
    run(source) {
      try {
        return context.run(source);
      } catch (error) {
        rethrow(error);
      }
    },

    clear() {
      context.clear();
    },

    depth() {
      return context.depth();
    },

    peek() {
      try {
        return context.peek();
      } catch (error) {
        rethrow(error);
      }
    },

    pop() {
      try {
        return context.pop();
      } catch (error) {
        rethrow(error);
      }
    },

    stack() {
      return context.stack();
    },
  };
};

/**
 * Format a Laskin value as a human-readable string.
 *
 * Uses the same formatting as the interpreter's `>string` word.
 */
export const laskinValueToString = async (value, options = {}) => {
  const module = await loadModule(options);

  try {
    return module.laskinValueToString(value);
  } catch (error) {
    rethrow(error);
  }
};

/**
 * Format a Laskin value as source code that can be evaluated again.
 *
 * Uses the same formatting as the interpreter's `>source` word.
 */
export const laskinValueToSource = async (value, options = {}) => {
  const module = await loadModule(options);

  try {
    return module.laskinValueToSource(value);
  } catch (error) {
    rethrow(error);
  }
};
