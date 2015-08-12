######################################################################
SET(CMAKE_VERBOSE_MAKEFILE on)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
######################################################################
ADD_EXECUTABLE(test_prog
               main.cpp
               base.cpp
               support.hpp
               supp_functions.cpp
               section.cpp
               symbol.cpp
               file.cpp)
TARGET_LINK_LIBRARIES(test_prog bfp)
######################################################################
IF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
  SETUP_TARGET_FOR_COVERAGE(coverage test_prog coverage)
ELSE ()
  ADD_TEST(Test ${VALGRIND} ${CMAKE_BINARY_DIR}/tests/test_prog --log_level=all --build_info)
ENDIF()
