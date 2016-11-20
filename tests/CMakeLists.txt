ADD_SUBDIRECTORY(c_prog)

SET(TEST_FILES
        main.cpp executable.cpp observer.cpp disassembler.cpp)

SET(TEST_HEADERS
        fixtures.hpp)

ADD_EXECUTABLE(run_tests ${TEST_FILES} ${TEST_HEADERS})

TARGET_LINK_LIBRARIES(run_tests gtest befa)

ADD_SUBDIRECTORY(test_cases)


ADD_TEST(unittests run_tests c_prog/sample)