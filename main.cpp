#include <befa/utils/bucket_allocator.hpp>

#include <befa.hpp>


std::string output;

struct NonRegisterArgPrinter
    : public symbol_table::VisitorBase {
  ASM_VISIT_ALL(arg) {
    output += arg->getName();
  }
};

struct TemporaryPrinter
    : public NonRegisterArgPrinter {
  ASM_VISIT_TEMPORARIES(arg) {
    auto left = arg->getLeft();
    if (left) left->accept(*this);
    output += arg->getOperator();
    if (!left)
      output += "(";
    arg->getRight()->accept(*this);
    if (!left)
      output += ")";
  }
};

struct ArgPrinter
    : public TemporaryPrinter {
  ASM_VISIT_REGISTERS(arg) {
    output += " <" + arg->type_name + ">(" + arg->getName() + ")";
  }

  ASM_VISIT_SIZED_TEMPORARIES(arg) {
    output += " <" + arg->type_name + ">";
    auto left = arg->getLeft();
    if (left) left->accept(*this);
    output += arg->getOperator();
    if (!left)
      output += "(";
    arg->getRight()->accept(*this);
    if (!left)
      output += ")";
  }
};

template<typename I>
std::string asHex(I integral) {
  std::stringstream ss;
  ss << std::hex << integral;
  return ss.str();
}

int main(int argc, const char **argv) {
  assert(argc == 2 && "missing path parameter");

  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));

  assert(file.isValid() && "file is not valid");

  std::string old_symbol;
  auto sym_table_symbols = file.generate_table();
  file.basic_block()
      .subscribe([&old_symbol, &sym_table_symbols](std::pair<
          std::shared_ptr<ExecutableFile::basic_block_type>,
          std::vector<ExecutableFile::instruction_type>
      > basic_block) {
        auto bb = basic_block.first;
        auto symbol = ptr_lock(bb->getParent());
        if (symbol->getName() != old_symbol) {
          output += "Symbol " + symbol->getName() +
              " <0x" + asHex(symbol->getAddress()) + ">:\n";
          old_symbol = symbol->getName();
        }
        output += "    BasicBlock #0x" + asHex(bb->getId()) + "\n";
        for (auto &instr : basic_block.second) {
          output += std::string("      <") + asHex(instr.getAddress()) +
              ">: " + instr.parse()[0] + " ";
          ArgPrinter arg_printer;
          for (auto &arg : instr.getArgs(sym_table_symbols)) {
            arg->accept(arg_printer);
            output += ", ";
          }
          output += "\n";
        }
      });
  file.runDisassembler();
  printf("%s\n", output.c_str());
  return 0;
}