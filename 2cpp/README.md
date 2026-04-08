# laskin2cpp

A silly experiment that transpiles Laskin programs into C++ that can then be
compiled into native executables. The focus is on maximum performance that
could eventually lead to discoveries of bottlenecks in the Laskin interpreter.

## TODO

Some additional optimizations that might be interesting to implement:

- Detection of numerical values when `context.lookup()` or `context.eval()` is
  called.
- If definition is immediately followed by `push` instruction, convert them
  into single combined instruction.

Each optimization should be optional that can be turned on/off with command
line parameters.
