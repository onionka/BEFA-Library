set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
set(SOURCE_FILES bfp.cpp)

add_library(bfp
			${SOURCE_FILES}
			exception.hpp
			bfp.hpp
			bfd.h)
TARGET_LINK_LIBRARIES(bfp bfd)