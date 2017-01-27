//
// Created by miro on 11/10/16.
//

#include "../../include/befa/llvm/instruction.hpp"
#include "../../include/befa/llvm/call.hpp"
#include "../../include/befa/llvm/cmp.hpp"
#include "../../include/befa.hpp"

void ExecutableFile::runDecompiler() {
  disassembly().subscribe([](instruction_type i ATTRIBUTE_UNUSED) {
    // parse instruction 'i'
  });

  runDisassembler();
}

namespace llvm {

// InstructionMapperBase
void InstructionMapperBase::register_factory(
    InstructionFactory *ptr
) {
  factories.push_back(ptr);
}

void InstructionMapperBase::remove_factory(
    InstructionFactory *ptr
) {
  factories.erase(std::remove(
      factories.begin(), factories.end(), ptr
  ), factories.end());
}
// InstructionMapperBase ~~~~~

// IFactoryBase
IFactoryBase::IFactoryBase(
    InstructionMapperBase &mapper
) : mapper(mapper) {
  mapper.register_factory(this);
}

IFactoryBase::~IFactoryBase() {
  mapper.remove_factory(this);
}
// IFactoryBase ~~~~~

const std::map<std::string, ICmpInstruction::types_e>
    ICmpInstruction::comparition_jumps{
    {"ja", GT},
    {"jg", GT},
    {"jnbe", GT},
    {"jnle", GT},

    {"jae", GE},
    {"jge", GE},
    {"jnb", GE},
    {"jnl", GE},

    {"jb", LT},
    {"jl", LT},
    {"jnae", LT},
    {"jnge", LT},

    {"jbe", LE},
    {"jle", LE},
    {"jna", LE},
    {"jng", LE},

    {"je", EQ},
    {"jne", NE},
};

InstructionMapperBase::InstructionMapperBase(
    const symbol_table_type &symbol_table
) : symbol_table(symbol_table) {}

//void InstructionMapper<CallInstruction>::operator()
//    (const ExecutableFile::instruction_type &i) {
//  auto parsed_i = i.parse();
//  std::vector<std::shared_ptr<symbol_table::VisitableBase>> args;
//  if (parsed_i[0] == "call" && (
//      // if this is call then get args
//      args = i.getArgs(symbol_table)
//  ).size() == 1) {
//    std::shared_ptr<InstructionFactory>
//    std::weak_ptr<ExecutableFile::symbol_type> call_target;
//    // call have only one argument (first)
//    invoke_accept(args[0], FunctionVisitor(call_target));
//    static_cast<factories::Factory <CallInstruction> &>
//    (factory).create(call_target, i);
//  }
//}
//
//void InstructionMapper<ICmpInstruction>::operator()
//    (const ExecutableFile::instruction_type &i) {
//  auto parsed_i = i.parse();
//  std::vector<std::shared_ptr<symbol_table::VisitableBase>> args;
//  ICmpInstruction::types_e type;
//  if (std::find_if(
//      ICmpInstruction::comparition_jumps.begin(),
//      ICmpInstruction::comparition_jumps.end(),
//      // find a type of instruction
//      [&](const std::pair<std::string, ICmpInstruction::types_e> &jmp) {
//        if (parsed_i[0] == jmp.first) {
//          type = jmp.second;
//          return true;
//        }
//        return false;
//      }) != ICmpInstruction::comparition_jumps.end()
//      && (
//          // if this is jump then get args
//          args = i.getArgs(symbol_table)
//      ).size() == 1) {
//    std::weak_ptr<symbol_table::Function::asm_symbol_type> jmp_target;
//    invoke_accept(args[0], FunctionVisitor(jmp_target));
//    static_cast<factories::Factory <CallInstruction> &>
//    (factory).create(jmp_target, i);
//  }
//}

}  // namespace llvm


