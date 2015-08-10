SET(CMAKE_VERBOSE_MAKEFILE on)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
######################################################################
SET(VALGRIND valgrind --leak-check=full --show-reachable=yes)
######################################################################
ADD_EXECUTABLE(test_prog_bin
               main.cpp)
TARGET_LINK_LIBRARIES(test_prog_bin bfp)
ADD_TEST(test_prog ${VALGRIND} ./test_prog_bin)