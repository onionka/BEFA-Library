######################################################################
ADD_EXECUTABLE(test_prog_bin
               main.cpp)
TARGET_LINK_LIBRARIES(test_prog_bin bfp)
ADD_TEST(test_prog ${VALGRIND} ./test_prog_bin)