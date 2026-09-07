import createLaskinModule from "./laskin.js";

/**
 * Error thrown by the Laskin interpreter.
 */
export class LaskinError extends Error {
  /**
   * @param {string} message
   * @param {string} type
   * @param {number} [line]
   * @param {number} [column]
   */
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

/**
 * @param {unknown} error
 * @returns {never}
 */
function rethrow(error) {
  if (
    error &&
    typeof error === "object" &&
    /** @type {{ name?: string }} */ (error).name === "LaskinError"
  ) {
    const e =
      /** @type {{ message: string, type: string, line?: number, column?: number }} */ (
        error
      );
    throw new LaskinError(e.message, e.type, e.line, e.column);
  }
  throw error;
}

/**
 * @param {import('./index.d.ts').LaskinContextNative} native
 * @returns {import('./index.d.ts').LaskinContext}
 */
function wrapContext(native) {
  return {
    run(source) {
      try {
        return native.run(source);
      } catch (error) {
        rethrow(error);
      }
    },
    clear() {
      native.clear();
    },
    depth() {
      return native.depth();
    },
    peek() {
      try {
        return native.peek();
      } catch (error) {
        rethrow(error);
      }
    },
    pop() {
      try {
        return native.pop();
      } catch (error) {
        rethrow(error);
      }
    },
    stack() {
      return native.stack();
    },
  };
}

/** @type {ReturnType<typeof createLaskinModule> | undefined} */
let defaultModulePromise;

/**
 * @param {import('./index.d.ts').CreateLaskinOptions} [options]
 */
function loadModule(options = {}) {
  const locateFile =
    typeof options.locateFile === "function"
      ? options.locateFile
      : (path) => new URL(path, import.meta.url).href;

  if (typeof options.locateFile !== "function") {
    defaultModulePromise ??= createLaskinModule({ locateFile });
    return defaultModulePromise;
  }

  return createLaskinModule({ locateFile });
}

/**
 * Initialize the WebAssembly module and create a new interpreter context.
 *
 * @param {import('./index.d.ts').CreateLaskinOptions} [options]
 * @returns {Promise<import('./index.d.ts').LaskinContext>}
 */
export async function createLaskin(options = {}) {
  const module = await loadModule(options);
  return wrapContext(new module.Context());
}

export default createLaskin;
