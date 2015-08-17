Binary File Parser Library
==========================
| TravisCI build | Codecov |
| :------------: | :-----: |
| [![Build Status](https://travis-ci.org/cibo94/Binary-File-Parser-Library.svg?branch=master)](https://travis-ci.org/cibo94/Binary-File-Parser-Library) | [![codecov.io](http://codecov.io/github/cibo94/Binary-File-Parser-Library/coverage.svg?branch=master)](http://codecov.io/github/cibo94/Binary-File-Parser-Library?branch=master) |

High-level C++ library makes analysis of binary file (executable, library) easier
in the light of performance and memory efficiency. This is superstructure of
opcodes and Binary file descriptor from binutils with better documentation and
implemented using design patters. Bachelor's work.

BUILD
=====

To build this project you will need to install following packages:
  - binutils-dev
  - cmake
  - lcov
  - g++ >= 4.9
  - boost-test >= 1.50.0
  - boost-program-options >= 1.50.0

Then build this with:
```bash
 $ cmake .
 $ make
 $ make man     # to build manual pages
 $ make html    # to build html documentation
```

TEST
====

To run tests:
```bash
 $ cmake .
 $ make
 $ make test ARGS='-V'
```

To see coverage you must change CMAKE_BUILD_TYPE to Coverage like this:
```bash
 $ cmake . -DCMAKE_BUILD_TYPE=Coverage
 $ make
 $ make coverage
```

INSTALL
=======

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

COVERAGE GRAPH:
---------------
![codecov.io](http://codecov.io/github/cibo94/Binary-File-Parser-Library/branch.svg?branch=master)
