SET(SOURCE_FILES
    parser.cpp
    exception.cpp
    symbol.cpp
    section.cpp
    file.cpp
    asm.cpp
    stream.cpp
    )

SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/include/bfp.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/exception.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/parser.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/file.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/section.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/asm.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/iterator.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/raw_vector.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/stream.hpp
    )

ADD_LIBRARY(bfp SHARED
            ${SOURCE_FILES}
            ${HEADER_FILES}
            ../include/bfp/line_info.hpp)

TARGET_LINK_LIBRARIES(bfp bfd opcodes ${Boost_LIBRARIES})

INSTALL(TARGETS bfp
        DESTINATION lib)