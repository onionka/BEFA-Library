######################################################################
ADD_EXECUTABLE(disassembler
               main.cpp)
TARGET_LINK_LIBRARIES(disassembler bfp ${Boost_LIBRARIES})
ADD_TEST(disassembler_test ${VALGRIND} ./disassembler disassembler --target elf64-x86-64 --list-symbols)