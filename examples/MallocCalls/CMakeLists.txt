######################################################################
ADD_EXECUTABLE(malloc_calls
               main.cpp)
TARGET_LINK_LIBRARIES(malloc_calls bfp ${Boost_LIBRARIES})
ADD_TEST(malloc_calls_test ${VALGRIND} ./malloc_calls malloc/malloc.out)

ADD_SUBDIRECTORY(malloc)