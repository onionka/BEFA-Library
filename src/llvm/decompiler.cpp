//
// Created by miro on 11/10/16.
//

#include "../../include/befa/llvm/instruction.hpp"
#include "../../include/befa/llvm/call.hpp"
#include "../../include/befa/llvm/cmp.hpp"
#include "../../include/befa.hpp"

void ExecutableFile::runDecompiler() {
  disassembly()
      .subscribe([&](
          instruction_type i ATTRIBUTE_UNUSED
      ) {
        // parse instruction 'i'
      });

  runDisassembler();
}

namespace llvm {

// InstructionMapperBase
void InstructionMapperBase::register_factory(
    const std::shared_ptr<InstructionFactory> &ptr
) { factories.push_back(ptr); }

void InstructionMapperBase::remove_factory(
    InstructionFactory *ptr
) {
  factories.erase(std::remove_if(
      factories.begin(), factories.end(),
      [&ptr] (const std::shared_ptr<InstructionFactory> &fac) {
        return fac.get() == ptr;
      }
  ), factories.end());
}
// InstructionMapperBase ~~~~~

// IFactoryBase
IFactoryBase::IFactoryBase(
    mapper_type mapper
) : mapper(mapper) {}

void IFactoryBase::deregister() { mapper->remove_factory(this); }
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
    const std::shared_ptr<symbol_table_type> &symbol_table
) : MapperForFactory(symbol_table) {}

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

// ~~~~~ CMP implementation
static std::map<CmpInstruction::types_e, std::string> type_to_str{
    {CmpInstruction::GT, "gt"},
    {CmpInstruction::GE, "ge"},
    {CmpInstruction::LT, "lt"},
    {CmpInstruction::LE, "le"},
    {CmpInstruction::UGT, "ugt"},
    {CmpInstruction::UGE, "uge"},
    {CmpInstruction::ULT, "ult"},
    {CmpInstruction::ULE, "ule"},
    {CmpInstruction::EQ, "eq"},
    {CmpInstruction::NE, "ne"}
};

std::string CmpInstruction::fetchSignature(
    const std::shared_ptr<symbol_table::VisitableBase> &result,
    const std::shared_ptr<symbol_table::VisitableBase> &lhs,
    CmpInstruction::types_e op,
    const std::shared_ptr<symbol_table::VisitableBase> &rhs
) const {

  std::string signature;
  symbol_table::SymbolVisitorL arg_visitor(
      [&signature](const symbol_table::Symbol *sym) {
        signature += sym->getName();
      }
  );
  lhs->accept(arg_visitor);
  signature += " " + type_to_str[op] + " ";
  rhs->accept(arg_visitor);
  return signature;
}
// ~~~~~ CMP implementation

}  // namespace llvm


