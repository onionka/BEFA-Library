SET(ASSEMBLY_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/instruction.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/symbol.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/basic_block.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/section.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/asm_arg_parser.hpp)

SET(ASSEMBLY_SOURCES
        ${PROJECT_SOURCE_DIR}/src/assembly/disassembler.cpp
        ${PROJECT_SOURCE_DIR}/src/assembly/executable_file.cpp
        ${PROJECT_SOURCE_DIR}/src/assembly/decoder.cpp)

SET(LLVM_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/llvm/instruction.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/llvm/unary.hpp
        ../include/befa/llvm/mapper.hpp)

SET(LLVM_SOURCES
        ${PROJECT_SOURCE_DIR}/src/llvm/decompiler.cpp)

SET(UTIL_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa/utils/visitor.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/algorithms.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/byte_array_view.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/observer.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/bucket_allocator.hpp)

ADD_LIBRARY(befa STATIC
        ${ASSEMBLY_SOURCES} ${ASSEMBLY_HEADERS}
        ${LLVM_SOURCES} ${LLVM_HEADERS}
        ${UTIL_HEADERS})

TARGET_LINK_LIBRARIES(befa ${BFD_LIBRARIES} ${PCRECPP_LIBRARIES})