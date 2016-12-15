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
    vi, vb, vrb, insf, ...\
) \
  template<typename DerivedT> \
  using vi = details::visitable_traits<__VA_ARGS__>::visitable_impl<DerivedT>; \
  using vb = details::visitable_traits<__VA_ARGS__>::visitable_base; \
  using vrb = details::visitable_traits<__VA_ARGS__>::visitor_base; \
  struct insf : public llvm::factories::details::fs<__VA_ARGS__> { };

#define ANONYMOUS_ENUM uint64_t

namespace factories {
/**
 * Template class for all factories
 */
template<typename /*T*/>
struct Factory {
// No need for virtual method, specialization will do the trick
//  Requirements:
//  "virtual" shared_ptr<T> create(
//      const vector<string> &input
//  ) = 0;
//  "virtual" vector<shared_ptr<T>> collect() = 0;
};

namespace details {
/**
 * Nasty hack to achieve partial, class, non-variable specialization
 */
template<typename ...>
struct parameter_tag {};

template<typename ...Is>
struct fs : public Factory<Is> ... {

  using instruction_base_vector =
  std::vector<std::shared_ptr<::VisitableBase<Is...>>>;

  instruction_base_vector collect() {
    instruction_base_vector result;
    collect(result, parameter_tag<Is...>());
    return result;
  }

 private:
  template<typename T, typename ...Ts>
  void collect(
      instruction_base_vector &result,
      parameter_tag<T, Ts...> &&
  ) {
    for (auto collected : Factory<T>::collect())
      result.emplace_back(collected);
    collect(result, parameter_tag<Ts...>());
  }

  /**
   * Stop iteration
   */
  void collect(
      instruction_base_vector &,
      parameter_tag<> &&
  ) {}
};
}  // namespace details
}  // namespace factories

/** Predeclaration */
struct InstructionFactory;

/**
 * Base of every instruction
 */
struct Instruction {
  using symbol_type = ExecutableFile::symbol_type;
  using instruction_type = ExecutableFile::instruction_type;
  using basic_block_type = ExecutableFile::basic_block_type;

  Instruction(const std::vector<instruction_type> &assembly)
      : assembly(assembly) {}

  const std::vector<instruction_type> &getAssembly() const {
    return assembly;
  }

  std::shared_ptr<basic_block_type> getParent() const {
    return assembly[0].getParent();
  }

  bfd_vma getAddress() const {
    return assembly[0].getAddress();
  }

 protected:
  std::vector<instruction_type> &get_assembly() { return assembly; }

 private:
  std::vector<instruction_type> assembly;
// No need for virtual functions ... because of visitor
//
// /** @return string representation of this LLVM instruction */
// virtual std::string getSignature() const = 0;
//
// /**
//  * @return address of this instruction (can be address of its assembly
//  *         base)
//  */
// virtual bfd_vma getAddress() const = 0;
//
// /**
//  * @returns origin of this instruction
//  */
// virtual const instruction_type &getAssembly() const /*override*/
//
// /**
//  * @return shared pointer to parent basic block
//  */
// virtual std::shared_ptr<
//      ExecutableFile::basic_block_type
// > getBasicBlock() = 0;
//
// /**
//  * @return pointer to basic block
//  */
// virtual std::shared_ptr<basic_block_type> getParent() const /*override*/
};

namespace mappers {
/**
 * Base for all mappers
 *
 * @tparam I type of instruction
 */
struct InstructionMapperBase {
  using visitable_type = std::shared_ptr<symbol_table::VisitableBase>;
  using symbol_table_type = ExecutableFile::symbol_map_type;

  InstructionMapperBase(
      InstructionFactory &factory,
      const symbol_table_type &sym_table
  );
 protected:
  InstructionFactory &factory;
  const symbol_table_type &symbol_table;
};

/**
 * Specializing this, instruction mapper will autoregister
 *
 * @tparam I instruction type
 */
template<typename I>
struct InstructionMapper {};
}  // namespace mappers


// ~~~~~ Instruction declarations ~~~~~
struct CallInstruction;

/** base for ICmp and FCmp */
struct CmpInstruction;

struct ICmpInstruction;

struct FCmpInstruction;
// ~~~~~ Instruction declarations ~~~~~


#define LLVM_VISIT_CMPs(visitable) \
    IMPLEMENT_VISIT(llvm::ICmpInstruction, visitable) \
      { visit_cmps_##visitable(visitable); } \
    IMPLEMENT_VISIT(llvm::FCmpInstruction, visitable) \
      { visit_cmps_##visitable(visitable); } \
  private: template<typename T> void visit_cmps_##visitable(const T *visitable)

#define LLVM_VISIT_ALL(visitable) \
    VISIT_(llvm::CallInstruction, visit_all_##visitable) \
    LLVM_VISIT_CMPs(visitable) { visit_all_##visitable(visitable); } \
  private: template<typename T> void visit_all_##visitable(const T *visitable)

namespace factories {

template<>
struct Factory<CallInstruction> {
  std::shared_ptr<CallInstruction> &create(
      std::weak_ptr<ExecutableFile::symbol_type> target,
      const ExecutableFile::instruction_type &parent
  );

  std::vector<std::shared_ptr<CallInstruction>> collect() {
    if (instruction)
      return {instruction};
    else
      return {};
  }

 private:
  std::shared_ptr<CallInstruction> instruction;
};

template<>
struct Factory<ICmpInstruction> {
  std::shared_ptr<ICmpInstruction> &create(
      std::weak_ptr<ExecutableFile::symbol_type> target,
      const ExecutableFile::instruction_type &parent
  );

  std::vector<std::shared_ptr<ICmpInstruction>> collect() {
    if (instruction)
      return {instruction};
    else
      return {};
  }

 private:
  std::shared_ptr<ICmpInstruction> instruction;
};

template<>
struct Factory<FCmpInstruction> {
  std::shared_ptr<FCmpInstruction> &create(
      std::weak_ptr<ExecutableFile::symbol_type> target,
      const ExecutableFile::instruction_type &parent
  );

  std::vector<std::shared_ptr<FCmpInstruction>> collect() {
    if (instruction)
      return {instruction};
    else
      return {};
  }

 private:
  std::shared_ptr<FCmpInstruction> instruction;
};
}  // namespace factories


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
    VisitableImpl, VisitableBase, VisitorBase, InstructionFactory,
    CallInstruction, ICmpInstruction, FCmpInstruction
)

namespace mappers {
template<>
struct InstructionMapper<CallInstruction>
    : public InstructionMapperBase {
  InstructionMapper(
      InstructionFactory &factory,
      const ExecutableFile::symbol_map_type &sym_table = {}
  ) : InstructionMapperBase(factory, sym_table) {}

  void operator()(const ExecutableFile::instruction_type &i);

 private:
  struct AddressCheck : public symbol_table::VisitorBase {
    ASM_VISIT_ALL(arg) {
      throw std::runtime_error(
          std::string("AddressCheck: Type: '") + typeid(arg).name() +
              "' is not valid parameter for Call (expecting Function)"
      );
    }
  };

  struct FunctionVisitor : public AddressCheck {
    using symbol_ptr = std::weak_ptr<symbol_table::Function::function_type>;

    FunctionVisitor(
        symbol_ptr &symbol_table
    ) : symbol(symbol_table) {}

    IMPLEMENT_VISIT(symbol_table::Function, arg) {
      symbol = arg->getAsmSymbol();
    }

   private:
    symbol_ptr &symbol;
  };
};

template<>
struct InstructionMapper<ICmpInstruction>
    : public InstructionMapperBase {
  InstructionMapper(
      InstructionFactory &factory,
      const ExecutableFile::symbol_map_type &sym_table = {}
  ) : InstructionMapperBase(factory, sym_table) {}

  void operator()(const ExecutableFile::instruction_type &i);
};
}  // namespace mappers

#ifndef INSTRUCTION_TEST
# undef REGISTER_VISITABLES
#endif

}  //  namespace llvm

#endif // BEFA_LLVM_LLVM_INSTRUCTION_HPP
