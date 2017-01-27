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
 * Prefer using IFactoryBase over this as a base class
 */
struct InstructionFactory {
  /**
   * Newly arrived instruction will notify every factory
   */
  virtual void notify(const ExecutableFile::instruction_type &) const = 0;
};

/**
 * Pre-declaration, used in IFactoryBase
 */
struct InstructionMapperBase;

/**
 * Adds self-registering/removal functionality to derived factories
 */
struct IFactoryBase
    : public InstructionFactory {

  /**
   * Self registering factories
   *
   * @param mapper on which this factory hooks on
   */
  IFactoryBase(InstructionMapperBase &mapper);

  /**
   * Self removal factory
   */
  virtual ~IFactoryBase();

  /**
   * Requirements for all Factories
   */
  using InstructionFactory::notify;

 protected:
  /**
   * @see InstructionMapperBase
   */
  InstructionMapperBase &mapper;
};

/**
 * Base class for a mapper
 *
 * @tparam I type of instruction
 */
struct InstructionMapperBase {
  using visitable_ptr = std::shared_ptr<VisitableBase>;
  using symbol_table_type = ExecutableFile::symbol_map_type;

  InstructionMapperBase(const symbol_table_type &sym_table);

  /**
   * Observer call hook
   *
   * @param i is assembly instruction
   * @default behaviour is notification of all factories
   */
  virtual void operator()(const ExecutableFile::instruction_type &i) const {
    for (auto &fac_ptr : factories) fac_ptr->notify(i);
  }

  /**
   * Appends newly allocated instruction
   * Factories use this to create new instructions
   * This should also check if instructions don't overlap
   *
   * @param ptr (will be destroyed after this ...)
   * @required
   */
  virtual void iappend(visitable_ptr &&ptr) = 0;

  std::shared_ptr<symbol_table::VisitableBase> find_symbol
      (const std::string &address) {
    bool result = false;
    std::stringstream ss;
    bfd_vma int_addr;

    ss << address;
    ss >> std::hex >> int_addr;

    auto visitor = [&result, &int_addr]
        (const symbol_table::Function *visitable) {
      result = visitable->getAsmSymbol()->getAddress() == int_addr;
    };
    for (auto &sym : symbol_table) {
      *sym.second >> visitor;
      if (result) return sym.second;
    }
    return nullptr;
  }

 public:

  /**
   * Mapper-scoped symbol table
   */
  const symbol_table_type &symbol_table;

 private:
  friend class IFactoryBase;

  /**
   * Adds factory into a list of factories
   *
   * @param ptr is pointer to a factory
   * @see factories
   */
  void register_factory(
      InstructionFactory *ptr
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
  std::vector<InstructionFactory *> factories;
};

struct InstructionMapper : InstructionMapperBase {
  InstructionMapper(const symbol_table_type &sym_table)
      : InstructionMapperBase(sym_table),
        traversed_addresses({}),
        append_traversed_addr([&](const Instruction *instr) {
          assert_ex(
              !contains(traversed_addresses, instr->getAddress()),
              "Duplicate instruction");
          traversed_addresses.push_back(instr->getAddress());
        }) {}

  /**
   * Appends newly allocated instruction
   *
   * @param ptr (will be destroyed after this ...)
   */
  virtual void iappend(visitable_ptr &&ptr) {
    invoke_accept(ptr, append_traversed_addr);
    created_instructions.get_subscriber()
        .on_next(ptr);
  }

  auto as_observable() const {
    return created_instructions.get_observable();
  }

 private:
  std::vector<bfd_vma> traversed_addresses;
  rxcpp::subjects::subject<visitable_ptr> created_instructions;
  InstructionVisitorL append_traversed_addr;
};

#ifndef INSTRUCTION_TEST
# undef REGISTER_VISITABLES
#endif

}  //  namespace llvm

#endif // BEFA_LLVM_LLVM_INSTRUCTION_HPP
