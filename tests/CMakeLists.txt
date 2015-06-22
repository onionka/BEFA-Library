set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")

######################################################################
add_executable(test1
               SampleTest1/test1.cpp)
TARGET_LINK_LIBRARIES(test1 bfp)
add_test(ExampleTest valgrind ./test1)

######################################################################
add_executable(test2
               SampleTest2/test2.cpp)
TARGET_LINK_LIBRARIES(test2 bfp)
add_test(ExampleTestFail valgrind ./test2)

######################################################################
add_library(sample SHARED
			plugin/sample.cpp
			plugin/sample.h)
add_executable(plugin_test
               PluginTest/plugin_test.cpp)
TARGET_LINK_LIBRARIES(plugin_test bfp)
add_test(PluginTest valgrind ./plugin_test)