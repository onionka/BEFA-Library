SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -pedantic")

SET(SOURCE_FILES
    bfd.cpp
    exception.cpp
    symbol.cpp
    section.cpp
    file.cpp
    )

SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/include/bfp.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/exception.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/bfd.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/file.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/section.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/support.hpp
    )

SET(TEMPLATE_FILES
    ${PROJECT_SOURCE_DIR}/include/bfp/support.tpp
    ${PROJECT_SOURCE_DIR}/include/bfp/support_helpers.tpp
    )

ADD_LIBRARY(bfp SHARED
            ${SOURCE_FILES}
            ${HEADER_FILES}
            ${TEMPLATE_FILES}
            )

TARGET_LINK_LIBRARIES(bfp bfd ${Boost_LIBRARIES})

INSTALL(TARGETS bfp
        DESTINATION lib)