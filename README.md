ELF++ Library
=============
| TravisCI build | Codecov |
| :------------: | :-----: |
| [![Build Status](https://travis-ci.org/cibo94/ELFpp-Library.svg?branch=master)](https://travis-ci.org/cibo94/ELFpp-Library) | [![codecov.io](http://codecov.io/github/cibo94/ELFpp-Library/coverage.svg?branch=master)](http://codecov.io/github/cibo94/ELFpp-Library?branch=master) |

High-level C++ library makes implementation of binary file (executable, library)
analysis of easier in the light of performance and memory efficiency.
A superstructure of opcodes and Binary file descriptor from binutils with
better documentation and implemented using design patters. Bachelor's work.

BUILD
=====

To build this project you will need to install following packages:
  - binutils-dev
  - cmake
  - g++ >= 4.9
  - boost-test >= 1.50.0 (tests)
  - lcov (tests)
  - boost-program-options >= 1.50.0 (testing program)

Then build this with:
```bash
 $ cmake .
 $ make
 $ make html    # to build html documentation
```

INSTALL
-------

```bash
 $ cmake . -DCMAKE_SYSTEM_PREFIX_PATH:PATH=/usr
 $ make
 $ sudo make install
```

With manual pages:
```bash
 $ cmake . -DWITH_MAN=1 -DCMAKE_SYSTEM_PREFIX_PATH:PATH=/usr
 $ make
 $ make man
 $ sudo make install
```

TEST
----

```bash
 $ cmake .
 $ make
 $ make test ARGS='-V'
```

COVERAGE GRAPH:
---------------
![codecov.io](http://codecov.io/github/cibo94/ELFpp-Library/branch.svg?branch=master)
