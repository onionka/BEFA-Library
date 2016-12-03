//
// Created by miro on 11/10/16.
//

#ifndef BEFA_LLVM_LLVM_INSTRUCTION_HPP
#define BEFA_LLVM_LLVM_INSTRUCTION_HPP

#include <tuple>

#include "../../befa.hpp"
#include "../utils/visitor.hpp"
#include "../utils/factory.hpp"
#include "function.hpp"
#include "basic_block.hpp"


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
  namespace details { template<typename ...Is> struct fs : Factory<Is>... {}; }\
  struct insf : public details::fs<__VA_ARGS__> { };

// ~~~~~ Instruction declarations ~~~~~
struct CallInstruction;

/* TODO: */
// ~~~~~ Instruction declarations ~~~~~

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

/**
 * Basic implementation of every instruction
 */
struct Instruction {
  /** @return string representation of this LLVM instruction */
  virtual std::string getSignature() const = 0;

  /**
   * @return address of this instruction (can be address of its assembly
   *         base)
   */
  virtual bfd_vma getAddress() const = 0;

  /**
   * @return shared pointer to parent basic block
   */
  virtual std::shared_ptr<BasicBlock> getBasicBlock() = 0;
};


struct LLVMBuilder {
  LLVMBuilder(Subject<std::shared_ptr<VisitableBase>> &i_subject)
      : i_subject(i_subject) {}

  void send_instruction(const std::shared_ptr<VisitableBase> &instr) {
    i_subject.update(instr);
  }

 private:
  std::vector<std::shared_ptr<VisitableBase>> instructions;
  Subject<std::shared_ptr<VisitableBase>> i_subject;
};


/**
 * Base for all mappers
 *
 * @tparam I type of instruction
 */
template<typename I>
struct InstructionMapperBase {
  InstructionMapperBase(
      InstructionFactory &factory,
      LLVMBuilder &builder
  ) : factory(factory), builder(builder) { }

 protected:
  Factory<I> &factory;
  LLVMBuilder &builder;
};

/**
 * This is unspecialized template for all mappers
 *
 * @tparam I
 */
template<typename I>
struct InstructionMapper { };


#ifndef INSTRUCTION_TEST
# undef REGISTER_VISITABLES
#endif

}  //  namespace llvm

#endif // BEFA_LLVM_LLVM_INSTRUCTION_HPP
