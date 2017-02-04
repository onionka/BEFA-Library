SET(ASSEMBLY_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/instruction.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/symbol.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/basic_block.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/section.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/instruction_parser.hpp)

SET(ASSEMBLY_SOURCES
        ${PROJECT_SOURCE_DIR}/src/assembly/disassembler.cpp
        ${PROJECT_SOURCE_DIR}/src/assembly/executable_file.cpp
        ${PROJECT_SOURCE_DIR}/src/assembly/decoder.cpp)

SET(LLVM_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/llvm/instruction.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/llvm/call.hpp
        ../include/befa/llvm/cmp.hpp ../include/befa/llvm/jmp.hpp)

SET(LLVM_SOURCES
        ${PROJECT_SOURCE_DIR}/src/llvm/decompiler.cpp)

SET(UTIL_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa/utils/visitor.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/algorithms.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/byte_array_view.hpp
        ../include/befa/utils/range.hpp ../include/befa/utils/assert.hpp ../include/befa/utils/backward.hpp ../include/befa/utils/types.hpp)

ADD_LIBRARY(befa STATIC
        ${ASSEMBLY_SOURCES} ${ASSEMBLY_HEADERS}
        ${LLVM_SOURCES} ${LLVM_HEADERS}
        ${UTIL_HEADERS})

TARGET_LINK_LIBRARIES(befa ${BFD_LIBRARIES} ${PCRECPP_LIBRARIES})