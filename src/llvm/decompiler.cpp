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

// ~~~~~ InstructionMapperBase
void InstructionMapperBase::register_factory(
    const std::shared_ptr<InstructionFactory> &ptr
) { factories.push_back(ptr); }

void InstructionMapperBase::remove_factory(
    InstructionFactory *ptr
) {
  factories.erase(std::remove_if(
      factories.begin(), factories.end(),
      [&ptr](const std::shared_ptr<InstructionFactory> &fac) {
        return fac.get() == ptr;
      }
  ), factories.end());
}
// ~~~~~ InstructionMapperBase

// ~~~~~ IFactoryBase
IFactoryBase::IFactoryBase(
    mapper_ptr mapper
) : mapper(mapper) {}

void IFactoryBase::deregister() { mapper->remove_factory(this); }
// ~~~~~ IFactoryBase

// ~~~~~ Mappers
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

auto InstructionMapperBase::operator()
    (const ExecutableFile::instruction_type &i) const {
  return rxcpp::sources::iterate(factories)
      // create new states of factories (and run then on - i instruction)
      .reduce(
          factory_ptr(), [&](
              factory_ptr acc,
              std::shared_ptr<InstructionFactory> fac_ptr
          ) {
            if (auto new_fact = fac_ptr->operator()(
                i, static_cast<const MapperForFactory *>(this)
            ))
              acc.push_back(new_fact);
            return acc;
          }
      )

          // filter out not-created factories
      .filter([&](const factory_ptr &o$) -> bool { return !o$.empty(); })

          // create new mapper from them by soft copying
      .map([&](
          const factory_ptr &o$
      ) -> std::shared_ptr<llvm::InstructionMapperBase> {
        return this->soft_copy(o$);
      });
}

std::shared_ptr<symbol_table::VisitableBase> MapperForFactory::find_symbol
    (const std::string &address) const {
  bool result = false;
  std::stringstream ss;
  bfd_vma int_addr;

  ss << address;
  ss >> std::hex >> int_addr;

  auto visitor = [&result, &int_addr]
      (const symbol_table::Function *visitable) {
    result = visitable->getAsmSymbol()->getAddress() == int_addr;
  };
  for (auto &sym : *symbol_table) {
    *sym.second >> visitor;
    if (result) return sym.second;
  }
  return nullptr;
}

bfd_vma MapperForFactory::get_address
    (const MapperForFactory::symbol_ptr &sym) const {
  const static bfd_vma non_address = (bfd_vma) -1;
  bfd_vma address = non_address;
  invoke_accept(sym, symbol_table::SymbolVisitorL([&address](
      const symbol_table::Symbol *s
  ) { address = s->getAddress(); }));
  assert_ex(address != non_address, "trying to load non-symbol address");
  return address;
}

std::shared_ptr<MapperForFactory::symbol_table_type>
MapperForFactory::get_symbol_table() const {
  return symbol_table;
}

InstructionMapper::observable_type InstructionMapper::observable() const {
  return created_instructions->get_observable();
}

InstructionMapper::subscriber_type InstructionMapper::subscriber() const {
  return created_instructions->get_subscriber();
}

rxcpp::subscription InstructionMapper::subscribe_on
    (InstructionMapper::i_observable_t o$) {
  using mapper_return_type = decltype(operator()(
      *(const ExecutableFile::instruction_type *) nullptr
  ));
  rxcpp::composite_subscription composite_subscription;
  composite_subscription.add(
      // run mapper on each instruction that will come
      o$.map([&](
              const ExecutableFile::instruction_type &i
          ) { return operator()(i); })
              // subscribe new-state mapper
              // subscribe again when factory returns new factory
          .subscribe([&](mapper_return_type new_mapper$) -> void {
            composite_subscription.add(
                new_mapper$.subscribe([&](
                    std::shared_ptr<llvm::InstructionMapperBase> new_mapper
                ) -> void { new_mapper->subscribe_on(o$); })
            );
          })
  );
  return composite_subscription;
}

InstructionMapper::InstructionMapper
    (const InstructionMapper::symbol_table_ptr &sym_table)
    : InstructionMapperBase(sym_table),
      traversed_addresses(std::make_shared<std::vector<bfd_vma>>()),
      created_instructions(std::make_shared<subject_type>()),
      append_traversed_addr([&](const Instruction *instr) {
        assert_ex(
            !contains(*traversed_addresses, instr->getAddress()),
            "Duplicate instruction");
        traversed_addresses->push_back(instr->getAddress());
      }) {}

InstructionMapper::InstructionMapper(
    const InstructionMapper &self,
    const std::vector<std::shared_ptr<InstructionFactory>> &o$
) : InstructionMapperBase(self, o$),
    traversed_addresses(self.traversed_addresses),
    created_instructions(self.created_instructions),
    append_traversed_addr(self.append_traversed_addr) {}

std::shared_ptr<InstructionMapperBase> InstructionMapper::soft_copy
    (std::vector<std::shared_ptr<InstructionFactory>> o$) const {
  return std::make_shared<InstructionMapper>(*this, o$);
}
// ~~~~~ Mappers

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
  invoke_accept(result, arg_visitor);
  signature += " = ";
  invoke_accept(lhs, arg_visitor);
  signature += " " + type_to_str[op] + " ";
  invoke_accept(rhs, arg_visitor);
  return signature;
}

CompareFactory::CompareFactory(const IFactoryBase::mapper_ptr &mapper)
    : IFactoryBase(mapper) {}

CompareFactory::next_factory_ptr CompareFactory::operator()(
    const ExecutableFile::instruction_type &instruction,
    const mapper_type *base
) const {
  if (instruction.getName() == "cmp") {
//     just one state
//    return std::make_shared<JumpFactory>();
  }
  return nullptr;
}
// ~~~~~ CMP implementation

}  // namespace llvm


