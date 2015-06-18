set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(test1
			   test1.cpp)
TARGET_LINK_LIBRARIES(test1 bfp)
add_test(ExampleTest test1 test1)

add_executable(test2
			   test2.cpp)
TARGET_LINK_LIBRARIES(test2 bfp)
add_test(ExampleTestFail test2 non_existing_file)