######################################################################
ADD_EXECUTABLE(symbol_reader
               main.cpp)
TARGET_LINK_LIBRARIES(symbol_reader bfp ${Boost_LIBRARIES})
ADD_TEST(symbol_reader__list_targets ${VALGRIND} ./symbol_reader symbol_reader --list-targets)
ADD_TEST(symbol_reader__list_symbols ${VALGRIND} ./symbol_reader symbol_reader --target elf64-x86-64 --list-symbols)