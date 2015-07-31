######################################################################
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
######################################################################
set(VALGRIND valgrind --leak-check=full --show-reachable=yes)
######################################################################
add_executable(test_prog
               TestProgram/main.cpp)
TARGET_LINK_LIBRARIES(test_prog bfp)
######################################################################
add_executable(functional
               Functional/main.cpp)
TARGET_LINK_LIBRARIES(functional bfp)
######################################################################
SETUP_TARGET_FOR_COVERAGE(coverage functional coverage)
