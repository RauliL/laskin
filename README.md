# Laskin

*Laskin* is a reverse polish notation calculator / programming language
inspired by [Forth], [RPL] and [Plorth].

## Requirements

* [GNU Multiple Precision Arithmetic Library]
* [CMake]
* C++17 compatible C++ compiler

Other dependencies already included in the source code repository as Git
submodules:

* [linenoise]
* [peelocpp-unicode]

## How to compile

```bash
mkdir build && cd build && cmake .. && make
```

[Forth]: https://en.wikipedia.org/wiki/Forth_%28programming_language%29
[RPL]: https://en.wikipedia.org/wiki/RPL_(programming_language)
[Plorth]: https://plorth.org
[GNU Multiple Precision Arithmetic Library]: https://gmplib.org/
[CMake]: https://cmake.org/
[linenoise]: https://github.com/antirez/linenoise
[peelocpp-unicode]: https://github.com/peelonet/peelocpp-unicode
