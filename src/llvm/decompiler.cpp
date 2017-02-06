//
// Created by miro on 11/10/16.
//

#include "../../include/befa/llvm/instruction.hpp"

// requirement classes
#include "../../include/befa/llvm/assignment.hpp"
#include "../../include/befa/llvm/unary_instruction.hpp"
#include "../../include/befa/llvm/jmp.hpp"
#include "../../include/befa/llvm/binary_operation.hpp"

// endpoint classes
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
    CmpInstruction::str_to_jmp{
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
const std::map<CmpInstruction::types_e, std::string>
    CmpInstruction::jmp_to_str{
    {GT, "ja"},
    {GT, "jg"},
    {GT, "jnbe"},
    {GT, "jnle"},

    {GE, "jae"},
    {GE, "jge"},
    {GE, "jnb"},
    {GE, "jnl"},

    {LT, "jb"},
    {LT, "jl"},
    {LT, "jnae"},
    {LT, "jnge"},

    {LE, "jbe"},
    {LE, "jle"},
    {LE, "jna"},
    {LE, "jng"},

    {EQ, "je"},
    {NE, "jne"},
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

instruction_parser::sym_map_t::type SymTable::to_map() const {
  return ::map(*symbol_table, [](const auto &item) {
    bfd_vma address;
    invoke_accept(item.second, symbol_table::SymbolVisitorL(
        [&address] (auto sym_ptr) {
          address = sym_ptr->getAddress();
        }));
    return std::make_pair(address, item.second);
  }, instruction_parser::sym_map_t::type());
}
// ~~~~~ Symbol Table
using sym_t = BinaryOperation::sym_t;
using ir_t = BinaryOperation::ir_t;

namespace details {
/**
 * @brief finds out operand name
 * @param operand
 * @return (name, true) if operand was Symbol
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
/**
 * @brief finds out instruction name
 * @param operand
 * @return (name, true) if operand was serializable
 *         ("", false) else
 */
std::tuple<std::string, bool> fetch_name(
    ir_t::ptr::shared operand
) {
  std::string result;
  bool fetched = false;
  if (operand)
    invoke_accept(operand, llvm::SerializableVisitorL(
        [&](const llvm::Serializable *instr) {
          result = instr->toString();
          fetched = true;
        }
    ));
  return std::make_tuple(result, fetched);
}
}  // namespace details

// ~~~~~ Binary operation impl
BinaryOperation::BinaryOperation(
    a_ir_t::vector::value assembly,
    sym_t::ptr::shared    target,
    sym_t::ptr::shared    lhs,
    bin_op_t::type        bin_op,
    sym_t::ptr::shared    rhs
) : assembly             (assembly)
  , target               (target)
  , lhs                  (lhs)
  , bin_op               (bin_op)
  , rhs                  (rhs) { }

sym_t::ptr::shared BinaryOperation::createResult(
    sym_t::ptr::shared    rhs,
    bin_op_t::type       _operator,
    sym_t::ptr::shared    lhs
) {
  return std::make_shared<symbol_table::Temporary>(
    rhs, _operator, lhs
  );
}

std::string BinaryOperation::fetch_name(
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

std::string BinaryOperation::toString() const {
  return Assignment::toString();
}

sym_t::ptr::shared BinaryOperation::getAssignee() const {
  return target;
}

sym_t::ptr::shared BinaryOperation::getAssigner() const {
  return createResult(lhs, getOperator(), rhs);
}

sym_t::vector::shared BinaryOperation::getDefinitions() const {
  return {target};
}

sym_t::vector::shared BinaryOperation::getUsedSymbols() const {
  return {lhs, rhs};
}

BinaryOperation::a_ir_t::vector::value BinaryOperation::getAssembly() const {
  return assembly;
}

BinaryOperation::operator_t::type BinaryOperation::getOperator() const {
  return bin_op;
}
// ~~~~~ Binary operation impl

// ~~~~~ Unary operator impl
UnaryInstruction::UnaryInstruction(
    a_ir_t::vector::value       assembly,
    sym_t::ptr::shared          target,
    operator_t::type           _operator,
    sym_t::ptr::shared          operand
) : assembly                   (assembly),
    target                     (target),
   _operator                  (_operator),
    operand                    (operand) {}

std::string UnaryInstruction:: fetchName(
    sym_t::ptr::shared target,
    operator_t::type _operator,
    sym_t::ptr::shared operand
) {
  return std::get<0>(details:: fetch_name (target))
      +                                    " = "
      +                                   _operator
      + std::get<0> (details:: fetch_name (operand));
}

std::string UnaryInstruction::toString() const {
  return Assignment::toString();
}

sym_t::ptr::shared UnaryInstruction::getAssignee() const {
  return target;
}

sym_t::ptr::shared UnaryInstruction::getAssigner() const {
  return getResult();
}

sym_t::vector::shared UnaryInstruction::getDefinitions() const {
  return {target};
}

sym_t::vector::shared UnaryInstruction::getUsedSymbols() const {
  return {operand};
}

UnaryInstruction::a_ir_t::vector::value UnaryInstruction::getAssembly() const {
  return assembly;
}

UnaryInstruction::operator_t::type UnaryInstruction::getOperator() const {
  return _operator;
}
// ~~~~~ Unary operator impl

// ~~~~~ Assignment
std::string Assignment::toString() const {
    return std::get<0>(details::fetch_name(getAssigner()))
    +                                        " = "
    +      std::get<0>(details::fetch_name(getAssignee()));
}

// ~~~~~ Assignment

// ~~~~~ CALL implementation
CallInstruction::CallInstruction(
    const a_vec_t&          assembly,
    sym_t::ptr::shared      result,
    sym_t::ptr::shared      call_target
) : UnaryInstruction      ({assembly},
                            result,
                            an_operator,
                            call_target) {}

std::string CallInstruction::toString() const {
  return std::get<0>(details::fetch_name(getDefinitions()[0]))
      + " = " + getOperator() + " "
      +  std::get<0>(details::fetch_name(getUsedSymbols()[0]))
      + "()";
}

const std::string CallInstruction::an_operator = "call";

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
       // if return value is integer under 32bits
       // it will be really there (workaround for now)
       auto call_i = std::make_shared<CallInstruction>(
           a_ir_t::vector::value({i}),
           symbol_table->get_or_create<
               symbol_table::SizedTemporary<symbol_table::types::DWORD>
           >("Temporary", "ResultOfTheCall"),
           target
       );
//       invoke_accept(eax, symbol_table::RegisterVisitorL(
//           [] (const symbol_table::RegisterBase *reg) {
//         printf("%s\n", reg->getName().c_str());
//       }));
       subscriber.on_next(
           std::static_pointer_cast<llvm::VisitableBase>(call_i)
       );
     });
}
// ~~~~~ CALL implementation

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

CmpInstruction::CmpInstruction(
    a_ir_t::c_info  ::ref      assembly,
    const sym_t::ptr::shared&  result,
    const sym_t::ptr::shared&  lhs,
    types_e                    op,
    const sym_t::ptr::shared&  rhs
) : BinaryOperation(
    {assembly}, result, lhs, type_to_str[op], rhs
) {}

std::string CmpInstruction::toString() const {
  auto args = getUsedSymbols();
  return std::get<0>(details::fetch_name(getAssignee()))
      + " = icmp " + getOperator() + " "
      + std::get<0>(details::fetch_name(args[0]))
      + ", "
      + std::get<0>(details::fetch_name(args[1]));
}

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

struct DummyInstruction : public Instruction {
  DummyInstruction(std::string __str) : __str(__str) {}

  a_ir_t::vector::value getAssembly() const override {
    return {};
  }

  std::string toString() const override {
    return __str;
  }
 private:
  std::string __str;
};

void CompareFactory::operator()(
    a_ir_t::c_info::ref instruction,
    sym_table_t::ptr::shared symbol_table,
    ir_t::rx::shared_subs subscriber
) const {
  auto name = instruction.getName();

  #define SEND_TEXT(result) \
  subscriber.on_next( \
    std::make_shared<llvm::DummyInstruction>(result))

  if (name == "cmp") {
    instruction
        .getArgs(symbol_table->to_map())
        .reduce(std::vector<sym_t::ptr::shared>(), [](
            std::vector<sym_t::ptr::shared> acc,
            sym_t::ptr::shared sym_ptr
        ) {
          acc.push_back(sym_ptr);
          return std::move(acc);
        })
        .subscribe([&](
            std::vector<sym_t::ptr::shared> args
        ) {
          auto zf = symbol_table->find_symbol("_zf");
          auto cf = symbol_table->find_symbol("_cf");
          assert_ex(
              zf && cf,
              "cannot continue without essential registers"
          );

          subscriber.on_next(std::make_shared<llvm::CmpInstruction>(
              instruction, zf, args[0], CmpInstruction::EQ, args[1]
          ));

          subscriber.on_next(std::make_shared<llvm::CmpInstruction>(
              instruction, cf, args[0], CmpInstruction::LT, args[1]
          ));
        });
  }

  if (name == "test") {
    instruction
        .getArgs(symbol_table->to_map())
        .reduce(std::vector<sym_t::ptr::shared>(), [](
            std::vector<sym_t::ptr::shared> acc,
            sym_t::ptr::shared sym_ptr
        ) {
          acc.push_back(sym_ptr);
          return std::move(acc);
        })
        .subscribe([&](
            std::vector<sym_t::ptr::shared> args
        ) {
          auto temporary = symbol_table->add_symbol
              <symbol_table::Temporary>(args[0], "&", args[1]);
          auto zf = symbol_table->find_symbol("_zf");
          auto sf = symbol_table->find_symbol("_sf");
          auto pf = symbol_table->find_symbol("_pf");
          assert_ex(
              zf && sf && pf,
              "cannot continue without essential registers"
          );

          subscriber.on_next(std::make_shared<llvm::UnaryInstruction>(
              a_ir_t::vector::value{instruction}, sf, "MSB", temporary
          ));

          subscriber.on_next(std::make_shared<llvm::CmpInstruction>(
               instruction, zf, temporary, CmpInstruction::EQ,
               std::make_shared<symbol_table::Immidiate>("0")
          ));

          subscriber.on_next(std::make_shared<llvm::UnaryInstruction>(
              a_ir_t::vector::value{instruction}, pf, "BitwiseXNOR", temporary
          ));
        });
  }
}
// ~~~~~ CMP implementation

}  // namespace llvm


