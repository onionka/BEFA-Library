#include <befa.hpp>

int main(int argc, const char **argv) {
  assert(argc == 2 && "missing path parameter");

  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));

  assert(file.isValid() && "file is not valid");

//  file.disassembly().subscribe([](ExecutableFile::instruction_type i) {
//    printf("\t%s\n", i.getDecoded().c_str());
//  });
//  printf("\n");

//  file.symbols()
//      .subscribe([](std::pair<
//          ExecutableFile::symbol_type,
//          std::vector<std::pair<
//              std::shared_ptr<ExecutableFile::basic_block_type>,
//              std::vector<ExecutableFile::instruction_type>
//          >>
//          > symbol) {
//
//      });
  std::string old_symbol;
  file.basic_block()
      .subscribe([&old_symbol](std::pair<
          std::shared_ptr<ExecutableFile::basic_block_type>,
          std::vector<ExecutableFile::instruction_type>
      > basic_block) {
        auto bb = basic_block.first;
        auto symbol = ptr_lock(bb->getParent());
        if (symbol->getName() != old_symbol) {
          printf("Symbol %s <0x%08x>:\n", symbol->getName().c_str(),
                 symbol->getAddress());
          old_symbol = symbol->getName();
        }
        printf("%3d. BasicBlock #0x%08lx\n", bb->getId(),
               basic_block.second.back().getAddress());
        for (auto &instr : basic_block.second) {
          printf(
              "    <%lX>: %s\n",
              instr.getAddress(),
              instr.getDecoded().c_str()
          );
        }
      });
  file.runDisassembler();
  return 0;
}