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
struct fs : Factory<Is> ... {

  using instruction_base_vector =
      std::vector<std::shared_ptr<::VisitableBase<Is...>>>;

  instruction_base_vector collect() {
    instruction_base_vector result;
    collect<Is...>(result, parameter_tag<Is...>());
    return result;
  }

 private:
  template<typename T, typename ...Ts>
  instruction_base_vector collect
      (instruction_base_vector &result,
       parameter_tag<T, Ts...> &&) {
    auto collected = Factory<T>::collect();
    result.insert(result.end(), collected.begin(), collected.end());
    return collect(result, parameter_tag<Ts...>());
  }

  /**
   * Stop iteration
   */
  instruction_base_vector collect
      (instruction_base_vector &result,
       parameter_tag<> &&) {
    return result;
  }

};
}  // namespace details
}  // namespace factories

/** Predeclaration */
struct InstructionFactory;

/**
 * Base of every instruction
 */
struct Instruction {
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

/* TODO: */
// ~~~~~ Instruction declarations ~~~~~

#define LLVM_VISIT_ALL(visitable) \
    VISIT_(llvm::CallInstruction, visit_##visitable) \
    template<typename T> void visit_##visitable(const T *visitable)

namespace factories {
template<>
struct Factory<CallInstruction> {
  std::shared_ptr<CallInstruction> &create(
      std::weak_ptr<ExecutableFile::symbol_type> target,
      const ExecutableFile::instruction_type &parent
  );

  std::vector<std::shared_ptr<CallInstruction>> collect() {
    return {instruction};
  }

 private:
  std::shared_ptr<CallInstruction> instruction;
};
}  // namespace factories


/** REGISTER INTSRUCTIONS HERE!
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
    CallInstruction
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
      symbol = arg->getCallee();
    }

   private:
    symbol_ptr &symbol;
  };
};
}  // namespace mappers

#ifndef INSTRUCTION_TEST
# undef REGISTER_VISITABLES
#endif

}  //  namespace llvm

#endif // BEFA_LLVM_LLVM_INSTRUCTION_HPP
