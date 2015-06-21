set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -pedantic")

set(SOURCE_FILES
    bfp.cpp)

add_library(bfp
            ${SOURCE_FILES}
            ${PROJECT_SOURCE_DIR}/include/bfp/exception.hpp
            ${PROJECT_SOURCE_DIR}/include/bfp/plugin_manager.hpp
            ${PROJECT_SOURCE_DIR}/include/bfp.hpp
            ${PROJECT_SOURCE_DIR}/include/bfd.h)
TARGET_LINK_LIBRARIES(bfp bfd)
INSTALL(TARGETS bfp
        DESTINATION lib)