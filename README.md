# Library for Binary Executable File Analysis 

## Requirements:
 - binutils-dev >= 2.2
 - cmake >= 3.2
 - g++ >= 6.0
 - pcrecpp
 - gtest
 - doxygen (doc)

## Building:
```
 $ mkdir build && pushd build
 $ cmake .. --DCMAKE_BUILD_TYPE=Release
 $ make -j4
 $ make test
 $ popd
```

## Project structure:
 - examples
 - include
   - befa
     - assembly
     - llvm
     - utils
 - src
   - assembly
   - llvm
 - tests
   - test_cases

