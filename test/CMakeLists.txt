set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")
set(SOURCE_FILES main.cpp)

add_executable(Binary_File_Parser
			   ${SOURCE_FILES}
			   ../bfp/exception.hpp
			   ../bfp/bfp.hpp
			   ../bfp/bfd.h)
TARGET_LINK_LIBRARIES(Binary_File_Parser bfp)