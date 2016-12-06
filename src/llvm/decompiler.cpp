//
// Created by miro on 11/10/16.
//

#include "../../include/befa/llvm/instruction.hpp"
#include "../../include/befa/llvm/unary.hpp"
#include "../../include/befa.hpp"

void ExecutableFile::runDecompiler() {
  disassembly().subscribe([](instruction_type i) {
    // parse instruction 'i'
  });

  runDisassembler();
}

namespace llvm {

namespace factories {

std::shared_ptr<CallInstruction> &Factory<CallInstruction>::create(
    std::weak_ptr<ExecutableFile::symbol_type> target,
    const ExecutableFile::instruction_type &parent
) {
  instruction = std::make_shared<CallInstruction>(target, parent);
  return instruction;
}
}  // namespace factories


namespace mappers {
InstructionMapperBase::InstructionMapperBase(
    InstructionFactory &factory,
    const symbol_table_type &symbol_table
) : factory(factory), symbol_table(symbol_table) {}

void InstructionMapper<CallInstruction>::operator()
    (const ExecutableFile::instruction_type &i) {
  auto parsed_i = i.parse();
  std::vector<std::shared_ptr<symbol_table::VisitableBase>> args;
  if (parsed_i[0] == "call" && (args = i.getArgs(symbol_table)).size() == 1) {
    std::weak_ptr<symbol_table::Function::function_type> call_target;
    invoke_accept(args[0], FunctionVisitor(call_target));
    factory.create(call_target, i);
  }
}
}  // namespace mappers


}  // namespace llvm


