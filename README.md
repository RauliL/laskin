# Laskin

*Laskin* is a reverse polish notation calculator / programming language
inspired by [Forth], [RPL] and [Plorth].

## Requirements

* [GNU Multiple Precision Arithmetic Library]
* [CMake]
* C++17 compatible C++ compiler

Other dependencies already included in the source code repository as Git
submodules:

* [peelocpp-chrono]
* [peelo-prompt]
* [peelo-unicode]

On Ubuntu, requirements for the compilation of this software can be installed
with this command:

```bash
$ sudo apt install build-essential cmake libgmp-dev
```

## How to compile

```bash
$ git clone https://github.com/RauliL/laskin.git
$ cd laskin
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ..
$ make
```

[Forth]: https://en.wikipedia.org/wiki/Forth_%28programming_language%29
[RPL]: https://en.wikipedia.org/wiki/RPL_(programming_language)
[Plorth]: https://plorth.org
[GNU Multiple Precision Arithmetic Library]: https://gmplib.org/
[CMake]: https://cmake.org/
[peelocpp-chrono]: https://github.com/peelonet/peelocpp-chrono
[peelo-prompt]: https://github.com/peelonet/peelo-prompt
[peelo-unicode]: https://github.com/peelonet/peelo-unicode
