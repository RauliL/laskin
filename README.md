# Laskin

[![build][build-image]][build-url]

_Laskin_ is a reverse polish notation calculator / programming language
inspired by [Forth], [RPL] and [Plorth].

[Try it in your browser.](https://rauli.dev/laskin-web/)

## Requirements

- [GNU MPFR Library]
- [CMake]
- C++17 compatible C++ compiler

Other dependencies that are handled by [CMake] are:

- [ordered-map]
- [peelo-chrono]
- [peelo-unicode]

On Ubuntu, requirements for the compilation of this software can be installed
with this command:

```bash
$ sudo apt install build-essential cmake libgmp-dev
```

## How to compile

```bash
$ git clone https://github.com/RauliL/laskin.git
$ cd laskin
$ mkdir build
$ cd build
$ cmake ..
$ make
```

[forth]: https://en.wikipedia.org/wiki/Forth_%28programming_language%29
[rpl]: https://en.wikipedia.org/wiki/RPL_(programming_language)
[plorth]: https://plorth.org
[GNU MPFR Library]: https://www.mpfr.org
[cmake]: https://cmake.org/
[ordered-map]: https://github.com/Tessil/ordered-map/
[peelo-chrono]: https://github.com/peelonet/peelo-chrono
[peelo-unicode]: https://github.com/peelonet/peelo-unicode
[build-image]: https://github.com/RauliL/laskin/actions/workflows/build.yml/badge.svg
[build-url]: https://github.com/RauliL/laskin/actions/workflows/build.yml
