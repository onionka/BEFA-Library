//
// Created by miro on 11/10/16.
//

#include "../../include/befa/llvm/instruction.hpp"
#include "../../include/befa/llvm/call.hpp"
#include "../../include/befa/llvm/cmp.hpp"
#include "../../include/befa.hpp"

void ExecutableFile::runDecompiler() {
  disassembly()
      .subscribe([&](inst_t::c_info::ref) {
        // parse instruction 'i'
      });

  runDisassembler();
}

namespace llvm {

// ~~~~~ Mappers
const std::map<std::string, CmpInstruction::types_e>
    CmpInstruction::comparition_jumps{
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

InstructionVisitorL create_traversal(
    InstructionMapper::addr_t::vector::value &addresses
) {
  return [&addresses](const Instruction *instr) {
    assert_ex(
        !contains(addresses, instr->getAddress()),
        "Duplicate instruction");
    addresses.push_back(instr->getAddress());
  };
}

InstructionMapper::InstructionMapper(
    sym_table_t::ptr::shared symbol_table
) : symbol_table(symbol_table),
    traversed_addresses({}),
    append_traversed_addr(create_traversal(traversed_addresses)) {

}

traits::ir::rx::shared_obs InstructionMapper::observable() const {
  return created_instructions.get_observable();
}

traits::ir::rx::shared_subs InstructionMapper::subscriber() const {
  return created_instructions.get_subscriber();
}

InstructionMapper::sym_table_t::rx::shared_obs InstructionMapper::reduce_instr(
    traits::a_ir::rx::obs o$
) {
  return o$
      .reduce(std::make_tuple(symbol_table, subscriber()), [&](
          std::tuple<
              sym_table_t::ptr::shared,
              ir_t::rx::shared_subs
          > acc,
          const traits::a_ir::info::type &i
      ) {
        for (auto &factory : factories)
          factory->operator()(
              i, std::get<0>(acc), std::get<1>(acc)
          );
        return acc;
      })
      .map([](
          std::tuple<
              sym_table_t::ptr::shared,
              ir_t::rx::shared_subs
          > acc
      ) { return std::get<0>(acc); });
}

void InstructionMapper::register_factory(fact_t::ptr::shared ptr) {
  factories.push_back(ptr);
}

void InstructionMapper::remove_factory(const fact_t::ptr::raw ptr) {
  factories.erase(std::remove_if(
      factories.begin(), factories.end(),
      [&ptr](const std::shared_ptr<LLVMFactory> &fac) {
        return fac.get() == ptr;
      }
  ), factories.end());
}

InstructionMapper::InstructionMapper(const InstructionMapper &self)
    : symbol_table(self.symbol_table),
      factories(self.factories),
      traversed_addresses(self.traversed_addresses),
      append_traversed_addr(self.append_traversed_addr),
      created_instructions(self.created_instructions) {}
// ~~~~~ Mappers

// ~~~~~ Symbol Table
traits::symbol::ptr::shared SymTable::find_symbol(
    const std::string &name
) const {
  bool result = false;
  auto visitor = symbol_table::SymbolVisitorL([&result, &name]
      (const symbol_table::Symbol *visitable) {
    result = (visitable->getName().find(name) != std::string::npos);
  });
  for (auto &sym : *symbol_table) {
    invoke_accept(sym.second, visitor);
    if (result) return sym.second;
  }
  return nullptr;
}

traits::symbol::ptr::shared SymTable::find_symbol(bfd_vma address) const {
  bool result = false;
  auto visitor = symbol_table::SymbolVisitorL([&result, &address]
      (const symbol_table::Symbol *visitable) {
    result = (visitable->getAddress() == address);
  });
  for (auto &sym : *symbol_table) {
    invoke_accept(sym.second, visitor);
    if (result) return sym.second;
  }
  return nullptr;
}

bfd_vma SymTable::get_address(SymTable::sym_t::ptr::shared sym) const {
  const static bfd_vma non_address = (bfd_vma) -1;
  bfd_vma address = non_address;
  invoke_accept(sym, symbol_table::SymbolVisitorL([&address](
      const symbol_table::Symbol *s
  ) { address = s->getAddress(); }));
  assert_ex(address != non_address, "trying to load non-symbol address");
  return address;
}

traits::sym_map::ptr::shared SymTable::to_map() const {
  return symbol_table;
}
// ~~~~~ Symbol Table

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

std::string CmpInstruction::fetch_name(
    const std::shared_ptr<symbol_table::VisitableBase> &result,
    const std::shared_ptr<symbol_table::VisitableBase> &lhs,
    CmpInstruction::types_e op,
    const std::shared_ptr<symbol_table::VisitableBase> &rhs
) {
  std::string signature;
  symbol_table::SymbolVisitorL arg_visitor(
      [&signature](const symbol_table::Symbol *sym) {
        signature += sym->getName();
      }
  );
  invoke_accept(result, arg_visitor);
  signature += " = ";
  invoke_accept(lhs, arg_visitor);
  signature += " " + type_to_str[op] + " ";
  invoke_accept(rhs, arg_visitor);
  return signature;
}

void CompareFactory::operator()(
    a_ir_t::c_info::ref instruction,
    sym_table_t::ptr::shared /*symboL_table*/,
    ir_t::rx::shared_subs subscriber
) const {
  auto name = instruction.getName();
  printf("%s\n", name.c_str());

#define SEND_TEXT(str) \
  subscriber.on_next(std::make_shared<llvm::Serializable>(str))

  if (name == "cmp") {
    SEND_TEXT("SF = MostSignificantBit(Temp)");
    SEND_TEXT("If (Temp == 0) ZF = 1 else ZF = 0");
    SEND_TEXT("PF = BitWiseXorNor(Temp[Max-1:0])");
    SEND_TEXT("CF = 0");
    SEND_TEXT("OF = 0");
    SEND_TEXT("AF is undefined");

  }

  if (name == "test") {
    SEND_TEXT("SF = MostSignificantBit(Temp)");
    SEND_TEXT("If (Temp == 0) ZF = 1 else ZF = 0");
    SEND_TEXT("PF = BitWiseXorNor(Temp[Max-1:0])");
    SEND_TEXT("CF = 0");
    SEND_TEXT("OF = 0");
    SEND_TEXT("AF is undefined");
  }
}
// ~~~~~ CMP implementation
using sym_t = BinaryOperator::sym_t;

namespace details {
/**
 * @brief finds out operand name
 * @param operand
 * @return (name, true) if operand was serializable
 *         ("", false) else
 */
std::tuple<std::string, bool> fetch_name(
    sym_t::ptr::shared operand
) {
  std::string result;
  bool fetched = false;
  if (operand)
    invoke_accept(operand, symbol_table::SymbolVisitorL(
        [&](const symbol_table::Symbol *symbol) {
          result = symbol->getName();
          fetched = true;
        }
    ));
  return std::make_tuple(result, fetched);
}
}  // namespace details

// ~~~~~ Binary operation impl
BinaryOperator::BinaryOperator(
    const inst_vect_t & assembly,
    sym_t::ptr::shared  target,
    sym_t::ptr::shared  lhs,
    bin_op_t::type      bin_op,
    sym_t::ptr::shared  rhs
) : Serializable       (fetch_name(
                          target, rhs,
                          bin_op, lhs
                       ))
  , Uses               (rhs, lhs)
  , Assignment         (assembly, target,
                        createResult(
                            rhs, bin_op, lhs
                        ))
  , Operator           (bin_op) {}

sym_t::ptr::shared BinaryOperator::createResult(
    sym_t::ptr::shared  rhs,
    bin_op_t::type     _operator,
    sym_t::ptr::shared  lhs
) {
  Operator op(_operator);
  return std::make_shared<symbol_table::Temporary>(
    rhs, op.toString(), lhs
  );
}

std::string BinaryOperator::fetch_name(
    sym_t::ptr::shared         target,
    sym_t::ptr::shared         lhs,
    bin_op_t::type            _operator,
    sym_t::ptr::shared         rhs
) {
  return std::get<0> (details::fetch_name (target))
      +                                    " = "
      +  std::get<0> (details::fetch_name (lhs))
      +                                   _operator
      +  std::get<0> (details::fetch_name (rhs));
}
// ~~~~~ Binary operation impl

// ~~~~~ Unary operator impl
UnaryInstruction::UnaryInstruction(
    const Instruction::a_vec_t &assembly,
    sym_t::ptr::shared          target,
    operator_t::type           _operator,
    sym_t::ptr::shared          operand
) : Serializable    (fetch_name(target, _operator, operand))
  , Defines                    (target)
  , Uses                       (operand)
  , Assignment                 (assembly, target,
                                create_result(
                                    _operator,
                                    operand
                                ))
  , Operator                   (_operator) {}

std::string UnaryInstruction:: fetch_name(
    sym_t::ptr::shared         target,
    operator_t::type          _operator,
    sym_t::ptr::shared         operand
) {
  return std::get<0>(details:: fetch_name (target))
      +                                    " = "
      +                                   _operator
      + std::get<0> (details:: fetch_name (operand));
}

sym_t::ptr::shared UnaryInstruction::create_result(
    operator_t::type _operator,
    sym_t::ptr::shared rhs
) {
  // TODO: add type of result ...
  return std::make_shared<
      symbol_table::Temporary
  >(_operator, rhs);
}
// ~~~~~ Unary operator impl

// ~~~~~ Assignment
Assignment::Assignment(
    const a_vec_t &          assembly,
    sym_t::ptr::shared       target,
    sym_t::ptr::shared       source
) : Serializable(fetch_name (target, source))
  , Defines                 (source)
  , Uses                    (target)
  , Instruction             (assembly) {}

std::string
Assignment           ::fetch_name(
    sym_t::ptr::shared       target,
    sym_t::ptr::shared       source
) {
  return std::get<0>(details::fetch_name(target))
       +                                 " = "
       + std::get<0>(details::fetch_name(source));
}

// ~~~~~ Assignment

// ~~~~~ CALL implementation
CallInstruction::CallInstruction(
    const a_vec_t&          assembly,
    sym_t::ptr::shared      call_target,
    sym_t::ptr::shared      result
) : Serializable(fetch_name(result, call_target))
  , UnaryInstruction      ({assembly},
                            result,
                           "call",
                            call_target) {}

std::string CallInstruction::fetch_name(
    sym_t::ptr::shared result,
    sym_t::ptr::shared target
) {
  return std::get<0>(details::fetch_name(result))
      + " = call "
      +  std::get<0>(details::fetch_name(target))
      + "()";
}

void CallFactory::operator()(
    a_ir_t::c_info::ref i,
    sym_table_t::ptr::shared symbol_table,
    ir_t::rx::shared_subs subscriber
) const {
  if (i.getName() == "call")
    // first parameter is target of call
    i.getArgs(symbol_table->to_map())
     .first()
     .subscribe([&](
         std::shared_ptr<symbol_table::VisitableBase> target
     ) {
       auto eax = symbol_table->find_symbol("_eax");
       invoke_accept(eax, symbol_table::RegisterVisitorL(
           [] (const symbol_table::RegisterBase *reg) {
         printf("%s\n", reg->getName().c_str());
       }));
       subscriber.on_next(
           std::static_pointer_cast<llvm::VisitableBase>(
               std::make_shared<CallInstruction>(
                   a_ir_t::vector::value({i}), target,
                   eax
               )
           )
       );
     });
}
// ~~~~~ CALL implementation

}  // namespace llvm


