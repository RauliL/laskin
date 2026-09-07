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

export function formatLaskinValue(value) {
  switch (value.type) {
    case "boolean":
      return value.value ? "true" : "false";
    case "number":
    case "string":
    case "quote":
    case "date":
    case "time":
    case "month":
    case "weekday":
      return value.value;
    case "vector":
      return value.value.map(formatLaskinValue).join(", ");
    case "record":
      return Object.entries(value.value)
        .map(([key, entry]) => `${key}=${formatLaskinValue(entry)}`)
        .join(", ");
  }
}

const rethrow = (error) => {
  if (error && typeof error === "object" && error.name === "LaskinError") {
    throw new LaskinError(error.message, error.type, error.line, error.column);
  }

  throw error;
};

/** @type {ReturnType<typeof createLaskinModule> | undefined} */
let defaultModulePromise;

/**
 * @param {import('./index.d.ts').CreateLaskinOptions} [options]
 */
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
