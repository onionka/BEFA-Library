set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -pedantic")

set(SOURCE_FILES
    bfp.cpp
    exception.cpp)

add_library(bfp SHARED
            ${SOURCE_FILES}
            ${PROJECT_SOURCE_DIR}/include/bfp.hpp
            ${PROJECT_SOURCE_DIR}/include/bfp/exception.hpp
            ${PROJECT_SOURCE_DIR}/include/bfp/bfp.hpp
            ${PROJECT_SOURCE_DIR}/include/bfp/support.tpp
            ${PROJECT_SOURCE_DIR}/include/bfp/support_helpers.tpp)
TARGET_LINK_LIBRARIES(bfp bfd ${Boost_LIBRARIES})
INSTALL(TARGETS bfp
        DESTINATION lib)