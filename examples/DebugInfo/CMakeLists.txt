######################################################################
ADD_EXECUTABLE(debug_info
               main.cpp)
TARGET_LINK_LIBRARIES(debug_info bfp)
ADD_TEST(debug_test ${VALGRIND} ./debug_info debug_info)