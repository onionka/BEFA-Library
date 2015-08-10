######################################################################
SET(CMAKE_VERBOSE_MAKEFILE on)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
######################################################################
ADD_EXECUTABLE(functional
               main.cpp
               base.cpp
               support.hpp
               supp_functions.cpp
               section.cpp
               symbol.cpp
               file.cpp)
TARGET_LINK_LIBRARIES(functional bfp)
######################################################################
SETUP_TARGET_FOR_COVERAGE(coverage functional coverage)
