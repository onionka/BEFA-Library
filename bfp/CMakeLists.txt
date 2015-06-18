set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -pedantic")
set(SOURCE_FILES bfp.cpp)

add_library(bfp
            ${SOURCE_FILES}
            exception.hpp
            bfp.hpp
            ../bfd/bfd.h)
TARGET_LINK_LIBRARIES(bfp bfd)