#include <befa.hpp>

int main(int argc, const char **argv) {
  assert(argc == 2 && "missing path parameter");

  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));

  assert(file.isValid() && "file is not valid");

  auto assembly$ = file.disassembly();
  assembly$.subscribe([](ExecutableFile::instruction_type i) {
    printf("\t%s\n", i.getDecoded().c_str());
  });
  printf("\n");

  file.basic_block().subscribe([] (std::vector<ExecutableFile::instruction_type> instructions) {
    std::shared_ptr<ExecutableFile::basic_block_type> bb = instructions.back().getParent();

    printf("\tBlock #%d\n", bb->getId());
    for (auto &instr : instructions) {
      printf("\t\t%s\n", instr.getDecoded().c_str());
    }
  });

  file.runDisassembler();
  return 0;
}