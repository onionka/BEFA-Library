#include <befa.hpp>


int main(int argc, const char **argv) {
  assert_ex(argc == 2, "missing path parameter");

  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));

  assert_ex(file.isValid(), "file is not valid");

  auto sym_table_symbols = file.generate_table();
  bfd_vma last_symbol = (bfd_vma) -1;
  bfd_vma last_section = (bfd_vma) -1;
  int last_bb = -1;

  file.disassembly()

          // prints section
      .map([&last_section, &last_symbol, &last_bb](
          const ExecutableFile::instruction_type &instruction
      ) {
        std::shared_ptr<ExecutableFile::section_type> section
            = ptr_lock(
                ptr_lock(
                    instruction.getParent()->getParent()
                )->getParent()
            );
        auto address = section->getAddress(nullptr);
        if (last_section != address) {
          printf("%s <0x%08lx>:\n",
                 section->getName().c_str(),
                 address);
          last_section = address;
          last_symbol = (bfd_vma) -1;
          last_bb = -1;
        }
        return instruction;
      })

          // prints symbol
      .map([&last_symbol, &last_bb](
          const ExecutableFile::instruction_type &instruction
      ) {
        std::shared_ptr<ExecutableFile::symbol_type> symbol
            = ptr_lock(instruction.getParent()->getParent());
        auto address = symbol->getAddress();
        if (last_symbol != address) {
          printf("  %s <0x%08lx>:\n",
                 symbol->getName().c_str(),
                 address);
          last_symbol = address;
          last_bb = -1;
        }
        return instruction;
      })

          // prints basic block
      .map([&last_bb](
          const ExecutableFile::instruction_type &instruction
      ) {
        std::shared_ptr<ExecutableFile::basic_block_type> bb
            = instruction.getParent();
        auto bb_id = bb->getId();
        if (last_bb != bb_id) {
          printf("    BasicBlock #%lu <0x%08lx>:\n",
                 bb->getId(), instruction.getAddress());
          last_bb = (int) bb_id;
        }
        return instruction;
      })

          // prints instruction
      .subscribe([](
          const ExecutableFile::instruction_type &instruction
      ) {
        printf("      <0x%08lx> %s\n",
               instruction.getAddress(),
               instruction.getDecoded().c_str());
      });

  file.runDisassembler();
  return 0;
}