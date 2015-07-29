######################################################################
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
######################################################################
set(VALGRIND valgrind --leak-check=full --show-reachable=yes)
######################################################################
#add_executable(test_prog
#               TestProgram/main.cpp)
#TARGET_LINK_LIBRARIES(test_prog bfp)
######################################################################
#add_executable(test_prog_fail
#               TestProgramFail/main.cpp)
#TARGET_LINK_LIBRARIES(test_prog_fail bfp)
######################################################################
#add_library(sample SHARED
#			Plugin/main.cpp
#			Plugin/main.h)
#add_executable(plugin_prog
#               PluginTestProgram/main.cpp)
#TARGET_LINK_LIBRARIES(plugin_prog bfp)
######################################################################
#add_test(TestProgram ${VALGRIND} ./test_prog)
#add_test(TestProgramFail ${VALGRIND} ./test_prog_fail)
#add_test(PluginTestProgram ${VALGRIND} ./plugin_prog)
######################################################################
add_executable(functional
               Functional/main.h
               Functional/main.cpp)
TARGET_LINK_LIBRARIES(functional bfp)
######################################################################
SETUP_TARGET_FOR_COVERAGE(coverage functional coverage)
