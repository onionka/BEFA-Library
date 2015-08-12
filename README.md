Binary File Parser (draft)  [![Build Status](https://travis-ci.org/cibo94/Binary-File-Parser-Library.svg?branch=master)](https://travis-ci.org/cibo94/Binary-File-Parser-Library) [![codecov.io](http://codecov.io/github/cibo94/Binary-File-Parser-Library/coverage.svg?branch=master)](http://codecov.io/github/cibo94/Binary-File-Parser-Library?branch=master)
==========================

High-level library that analyse binary file (executable) for testing purposes. Bachelor's work.

![codecov.io](http://codecov.io/github/cibo94/Binary-File-Parser-Library/branch.svg?branch=master)

BUILD
=====

To build this project you will need to install:
  - binutils-dev
  - cmake
  - g++ >= 4.9
  - boost-test >= 1.50.0
  - lcov >= 1.11

Then run following commands:
```bash
 $ cmake .
 $ make
 $ make man
 $ sudo make install
```

In ```cmake .``` use ```-DWITH_MAN``` option to build and install manual page