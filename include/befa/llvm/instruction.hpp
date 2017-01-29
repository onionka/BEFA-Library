//
// Created by miro on 11/10/16.
//

#ifndef BEFA_LLVM_LLVM_INSTRUCTION_HPP
#define BEFA_LLVM_LLVM_INSTRUCTION_HPP

#include <tuple>
#include <memory>

#include "../utils/visitor.hpp"
#include "../../befa.hpp"

namespace llvm {

/**
 * @param vi visitable implementation
 * @param vb visitable base
 * @param vrb visitor base
 * @param insf instruction factory
 * @see VisitableBase
 * @see VisitableImpl
 * @see VisitorBase
 * @see Factory
 */
#define REGISTER_VISITABLES(\
    vi, vb, vrb, ...\
) template<typename DerivedT> \
  using vi = visitable_traits<__VA_ARGS__>::visitable_impl<DerivedT>; \
  using vb = visitable_traits<__VA_ARGS__>::visitable_base; \
  using vrb = visitable_traits<__VA_ARGS__>::visitor_base;

// ~~~~~ Instruction declarations ~~~~~
/**
 * Base of every instruction
 */
struct Instruction;

struct CallInstruction;

//struct InstrictInstruction;

struct CmpInstruction;

struct ICmpInstruction;

struct FCmpInstruction;

struct TerminatorInstruction;

struct JumpInstruction;

struct BranchInstruction;

struct Serializable;
// ~~~~~ Instruction declarations ~~~~~


#define LLVM_COMPARE_LIST \
  FCmpInstruction, ICmpInstruction

#define LLVM_TERMINATOR_LIST \
  JumpInstruction, BranchInstruction

#define LLVM_INSTRUCTION_LIST \
  CallInstruction, CmpInstruction, LLVM_COMPARE_LIST, \
  TerminatorInstruction, LLVM_TERMINATOR_LIST

#define LLVM_VISITABLES \
  Instruction, Serializable, LLVM_INSTRUCTION_LIST

// ~~~~~ Generalizations ~~~~~
using CompareVisitorL = LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, CmpInstruction,
    LLVM_COMPARE_LIST
>;

using BranchVisitorL = LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, TerminatorInstruction,
    LLVM_TERMINATOR_LIST
>;

using InstructionVisitorL = LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, Instruction,
    LLVM_INSTRUCTION_LIST
>;

using SerializableVisitorL = LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, Serializable,
    LLVM_INSTRUCTION_LIST
>;

using CompareVisitor = typename CompareVisitorL::Base;
using BranchVisitor = typename BranchVisitorL::Base;
using InstructionVisitor = typename InstructionVisitorL::Base;
using SerializableVisitor = typename SerializableVisitorL::Base;

// ~~~~~ Generalizations ~~~~~

/** REGISTER INSTRUCTIONS HERE!
 *
 * visitable_impl Use this as a base class for every
 *                final instruction class

 * visitable_base Use this as a base class for every
 *                non-final instruction class
 *
 * visitor_base Use this as a base class for every
 *              visitor of instruction class
 */
REGISTER_VISITABLES(
    VisitableImpl, VisitableBase, VisitorBase,
    LLVM_VISITABLES
)

using InstructionTraits = visitable_traits<LLVM_VISITABLES>;

struct Serializable : virtual public VisitableBase {
  Serializable(std::string signature) : signature(signature) {}

  const std::string &getSignature() const { return signature; }

 private:
  std::string signature;
};

/**
 * Base class for every instruction
 *
 * You can traverse this via Generalizer
 * @see Generalizer
 */
struct Instruction : virtual public VisitableBase {
  using symbol_type = ExecutableFile::symbol_type;
  using instruction_type = ExecutableFile::instruction_type;
  using instruction_ptr = std::shared_ptr<instruction_type>;
  using basic_block_type = ExecutableFile::basic_block_type;

  Instruction(
      const std::vector<instruction_type> &assembly
  ) : assembly(assembly) {}

  const std::vector<instruction_type> &getAssembly() const {
    return assembly;
  }

  std::shared_ptr<basic_block_type> getParent() const {
    return assembly[0].getParent();
  }

  bfd_vma getAddress() const {
    return assembly[0].getAddress();
  }

  void accept(VisitorBase &visitor) override {
    visitor.visit(this);
  }

 protected:
  /**
   * Almost always this will be only one instruction
   * (but occasionally multiple)
   */
  std::vector<instruction_type> assembly;
};

/**
 * Pre-declaration, used in IFactoryBase
 */
struct InstructionMapperBase;

struct MapperForFactory;

/**
 * Prefer using IFactoryBase over this as a base class
 */
struct InstructionFactory {
  /**
   * Newly arrived instruction will notify every factory
   */
  virtual void notify(
      const ExecutableFile::instruction_type &,
      const InstructionMapperBase *
  ) const = 0;
};

/**
 * adds subject, but still requires notify
 */
struct IFactoryBase
    : public InstructionFactory {

  using mapper_type = std::shared_ptr<llvm::MapperForFactory>;

  /**
   * Self registering factories
   *
   * @param subject is a subject that will stream instructions
   */
  IFactoryBase(
      mapper_type mapper
  );

  /**
   * Polymorphism requirement
   */
  virtual ~IFactoryBase() = default;

  /**
   * If you want to remove from mapper factories, call this method
   */
  void deregister();

  /**
   * Requirements for all Factories
   */
  using InstructionFactory::notify;

 protected:
  mapper_type mapper;
};

/**
 * Interface window for factories
 */
struct MapperForFactory {
  // ~~~~~ Symbol types
  using symbol_type = symbol_table::VisitableBase;
  using symbol_ptr = std::shared_ptr<symbol_type>;
  using symbol_table_type = ExecutableFile::symbol_map_type;
  using symbol_table_ptr = std::shared_ptr<symbol_table_type>;
  // ~~~~~ Symbol types

  // ~~~~~ llvm::Instruction types
  using instruction_type = VisitableBase;
  using instruction_ptr = std::shared_ptr<instruction_type>;
  using subject_type = rxcpp::subjects::subject<instruction_ptr>;
  using subscriber_type = subject_type::subscriber_type;
  using observable_type = subject_type::observable_type;
  // ~~~~~ llvm::Instruction types

  /**
   * Create mapper with symbol table
   */
  MapperForFactory(const symbol_table_ptr &symbol_table)
      : symbol_table(symbol_table) {}

  /**
   * Fetches symbol by its address
   * @param address in string format
   * @return pointer to VisitableBase or nullptr
   */
  std::shared_ptr<symbol_table::VisitableBase> find_symbol
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

  /**
   * Append new symbol
   */
  void append_symbol(const symbol_ptr &sym) {
    symbol_table->operator[](get_address(sym)) = sym;
  }

  /**
   * Removes factory from a list of factories by value
   *
   * @param ptr is pointer to a factory
   * @see factories
   */
  virtual void remove_factory(
      InstructionFactory *ptr
  ) = 0;

  virtual subscriber_type subscriber() const = 0;

  /**
   *
   * @param sym
   * @return
   */
  bfd_vma get_address(const symbol_ptr &sym) const {
    const static bfd_vma non_address = (bfd_vma) -1;
    bfd_vma address = non_address;
    invoke_accept(sym, symbol_table::SymbolVisitorL([&address](
      const symbol_table::Symbol *s
    ) { address = s->getAddress(); }));
    assert_ex(address != non_address, "trying to load non-symbol address");
    return address;
  }

  /**
   * Mapper-scoped symbol table
   */
  std::shared_ptr<symbol_table_type> symbol_table;
};

/**
 * Base class for a mapper
 *
 * @tparam I type of instruction
 */
struct InstructionMapperBase : public MapperForFactory {
  InstructionMapperBase(const symbol_table_ptr &sym_table);

  /**
   * Observer call hook
   *
   * @param i is assembly instruction
   * @default behaviour is notification of all factories
   */
  virtual void operator()(const ExecutableFile::instruction_type &i) const {
    for (auto &fac_ptr : factories) fac_ptr->notify(i, this);
  }

  auto getMapper() {
    return [&] (const ExecutableFile::instruction_type &i) {
      this->operator()(i);
    };
  }

  /**
   * Expects that derivations of this will contain observable of instructions
   */
  virtual observable_type observable() const = 0;

  /**
   * Adds factory into a list of factories
   *
   * @param ptr is pointer to a factory
   * @see factories
   */
  void register_factory(
      const std::shared_ptr<InstructionFactory> &ptr
  );

  /**
   * Removes factory from a list of factories by value
   *
   * @param ptr is pointer to a factory
   * @see factories
   */
  void remove_factory(
      InstructionFactory *ptr
  );

 protected:
  std::vector<std::shared_ptr<InstructionFactory>> factories;
};

struct InstructionMapper : public InstructionMapperBase {
  InstructionMapper(const symbol_table_ptr &sym_table)
      : InstructionMapperBase(sym_table),
        traversed_addresses({}),
        append_traversed_addr([&](const Instruction *instr) {
          assert_ex(
              !contains(traversed_addresses, instr->getAddress()),
              "Duplicate instruction");
          traversed_addresses.push_back(instr->getAddress());
        }) {}

  observable_type observable() const override {
    return created_instructions.get_observable();
  }

  subscriber_type subscriber() const override {
    return created_instructions.get_subscriber();
  }

 private:
  std::vector<bfd_vma> traversed_addresses;
  subject_type created_instructions;
  InstructionVisitorL append_traversed_addr;
};

#ifndef INSTRUCTION_TEST
# undef REGISTER_VISITABLES
#endif

}  //  namespace llvm

#endif // BEFA_LLVM_LLVM_INSTRUCTION_HPP
