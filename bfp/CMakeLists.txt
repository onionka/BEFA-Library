set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -pedantic")

set(SOURCE_FILES
    bfp.cpp
    exception.cpp)

add_library(bfp
            ${SOURCE_FILES}
            ../include/bfp.hpp
            ../include/bfp/exception.hpp
            ../include/bfp/bfp.hpp
            ${BFD_INCLUDE_DIR}/bfd.h)
TARGET_LINK_LIBRARIES(bfp bfd ${Boost_LIBRARIES})
INSTALL(TARGETS bfp
        DESTINATION lib)