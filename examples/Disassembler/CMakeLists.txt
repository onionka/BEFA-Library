######################################################################
ADD_EXECUTABLE(disassembler
               main.cpp)
TARGET_LINK_LIBRARIES(disassembler bfp)
ADD_TEST(disassembler_test ${VALGRIND} ./disassembler disassembler)