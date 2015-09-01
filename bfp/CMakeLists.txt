SET(SOURCE_FILES
    parser.cpp
    exception.cpp
    symbol.cpp
    section.cpp
    file.cpp
    asm.cpp
    )

SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/include/bfp.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/exception.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/parser.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/file.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/section.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/symbol.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/support.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/asm.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/iterator.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/raw_vector.hpp
    ${PROJECT_SOURCE_DIR}/include/bfp/stream.hpp
    )

SET(TEMPLATE_FILES
    ${PROJECT_SOURCE_DIR}/include/bfp/support.tpp
    ${PROJECT_SOURCE_DIR}/include/bfp/support_helpers.tpp
    )

ADD_LIBRARY(bfp SHARED
            ${SOURCE_FILES}
            ${HEADER_FILES}
            ${TEMPLATE_FILES}
            stream.cpp)

TARGET_LINK_LIBRARIES(bfp bfd opcodes ${Boost_LIBRARIES})

INSTALL(TARGETS bfp
        DESTINATION lib)