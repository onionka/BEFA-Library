//
// Created by miro on 11/29/16.
//
#include <befa.hpp>


struct Disassembler : Observer<std::pair<
    std::shared_ptr<ExecutableFile::basic_block_type>,
    std::vector<ExecutableFile::instruction_type>
>> {
  void operator() (const value_type &basic_block) {
    auto bb = basic_block.first;
    auto symbol = ptr_lock(bb->getParent());
    if (symbol->getAddress() != previous_address) {
      printf("Symbol %s <0x%08lX>:\n",
             symbol->getName().c_str(),
             symbol->getAddress());
      previous_address = symbol->getAddress();
    }
    printf("  <0x%08lX> BasicBlock:\n",
           bb->getId());
    for (auto &instr : basic_block.second) {
      printf(
          "    <0x%08lX>: %s\n",
          instr.getAddress(),
          instr.getDecoded().c_str()
      );
    }
  }

 private:
  uint64_t previous_address;
};


int main(int argc, char **argv) {
  while (argc > 1) {
    auto executable_file = ExecutableFile::open(*(argv + (argc - 1)));
    auto disassembler = Disassembler();
    executable_file.basic_block() >> disassembler;
    executable_file.runDisassembler();
    --argc;
  }
  return 0;
}
