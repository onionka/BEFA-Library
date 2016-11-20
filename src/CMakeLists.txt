SET(ASSEMBLY_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/instruction.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/symbol.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/basic_block.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/assembly/section.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/utils/observer.hpp)

SET(ASSEMBLY_SOURCES
        assembly/disassembler.cpp
        assembly/executable_file.cpp)

SET(LLVM_HEADERS
        ${PROJECT_SOURCE_DIR}/include/befa.hpp
        ${PROJECT_SOURCE_DIR}/include/befa/llvm/instruction.hpp)

SET(LLVM_SOURCES
        llvm/decompiler.cpp)

ADD_LIBRARY(befa STATIC
        ${ASSEMBLY_SOURCES} ${ASSEMBLY_HEADERS}
        ${LLVM_SOURCES} ${LLVM_HEADERS} ../include/befa/utils/algorithms.hpp ../include/befa/utils/byte_array_view.hpp)

TARGET_LINK_LIBRARIES(befa ${BFD_LIBRARIES} ${PCRECPP_LIBRARIES})