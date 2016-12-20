#include <befa.hpp>


struct NonRegisterArgPrinter
    : public symbol_table::VisitorBase {
  ASM_VISIT_ALL(arg) {
    printf("%s", arg->getName().c_str());
  }
};

struct TemporaryPrinter
    : public NonRegisterArgPrinter {
  ASM_VISIT_TEMPORARIES(arg) {
    auto left = arg->getLeft();
    if (left) left->accept(*this);
    printf("%s", arg->getOperator().c_str());
    if (!left)
      printf("(");
    arg->getRight()->accept(*this);
    if (!left)
      printf(")");
  }
};

struct ArgPrinter
    : public TemporaryPrinter {
  ASM_VISIT_REGISTERS(arg) {
    printf(" <%s>%s ", arg->type_name.c_str(), arg->getName().c_str());
  }

  ASM_VISIT_SIZED_TEMPORARIES(arg) {
    printf(" <%s>", arg->type_name.c_str());
    auto left = arg->getLeft();
    if (left) left->accept(*this);
    printf("%s", arg->getOperator().c_str());
    if (!left)
      printf("(");
    arg->getRight()->accept(*this);
    if (!left)
      printf(")");
  }
};

using basic_block_type = std::pair<
    std::shared_ptr<ExecutableFile::basic_block_type>,
    std::vector<ExecutableFile::instruction_type>
>;

int main(int argc, const char **argv) {
  assert(argc == 2 && "missing path parameter");

  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));

  assert(file.isValid() && "file is not valid");

  std::string old_symbol;
  auto sym_table_symbols = file.generate_table();
  file.basic_block()
      // parse symbol
      >> [&old_symbol](basic_block_type basic_block) {
        auto bb = basic_block.first;
        auto symbol = ptr_lock(bb->getParent());
        if (symbol->getName() != old_symbol) {
          printf("Symbol %s <0x%08lx>:\n", symbol->getName().c_str(),
                 symbol->getAddress());
          old_symbol = symbol->getName();
        }
      }
      // parse basic block
      >> [&sym_table_symbols](basic_block_type basic_block) {
        auto bb = basic_block.first;
        printf("    BasicBlock #0x%08lx\n", bb->getId());
        // print instruction
        for (auto &instr : basic_block.second) {
          printf(
              "      <%lX>: %s ",
              instr.getAddress(),
              instr.parse()[0].c_str()
          );
          // print parameters
          ArgPrinter arg_printer;
          for (auto &arg : instr.getArgs(sym_table_symbols)) {
            arg->accept(arg_printer);
            printf(", ");
          }
          printf("\n");
        }
      };
  file.runDisassembler();
  return 0;
}