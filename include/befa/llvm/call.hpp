//
// Created by miro on 11/30/16.
//

#ifndef BEFA_LLVM_UNARY_HPP
#define BEFA_LLVM_UNARY_HPP

#include "instruction.hpp"
#include "../../befa.hpp"

namespace llvm {
struct CallInstruction
    : virtual public VisitableBase,
        // traits
      public Instruction,
      public Serializable {
  using symbol_type = symbol_table::VisitableBase;
  using symbol_ptr = std::shared_ptr<symbol_type>;
  using instruction_type = ExecutableFile::instruction_type;
  using basic_block_type = ExecutableFile::basic_block_type;

  CallInstruction(
      symbol_ptr result,
      symbol_ptr target,
      const instruction_type &assembly
  ) : Instruction({assembly}), Serializable(fetchSignature(result, target)),
      target(target), result(result) {}

  symbol_ptr getTarget() const {
    return target;
  }

  symbol_ptr getResult() const {
    return result;
  }

  void accept(VisitorBase &visitor) override {
    visitor.visit(this);
  }

 private:

  using generalizer_t = LambdaGeneralizer<
      symbol_table::VisitorTraits,
      symbol_table::Symbol,
      ASM_SYMBOLS_LIST
  >;

  generalizer_t create_name_visitor(std::string &result) const {
    return generalizer_t(
        [&result](const auto &ptr) { result = ptr->getName(); }
    );
  }

  std::string fetchSignature(
      symbol_ptr result,
      symbol_ptr target
  ) const {
    std::string name, result_name;
    if (target)
      invoke_accept(target, create_name_visitor(name));
    if (result)
      invoke_accept(result, create_name_visitor(result_name));
    return result_name + " = call @" + name + "()";
  }

  symbol_ptr target;
  symbol_ptr result;
};

struct CallFactory : public IFactoryBase {
  CallFactory(InstructionMapperBase &mapper)
      : IFactoryBase(mapper) {}

  void notify(const ExecutableFile::instruction_type &i) const override {
    auto pieces = i.parse();
    if (pieces[0] == "call") {
      mapper.iappend(std::static_pointer_cast<VisitableBase>(
          std::make_shared<CallInstruction>(
              nullptr,
              mapper.find_symbol(pieces[1]),
              i
          )
      ));
    }
  }
};

}  // namespace llvm

#endif //BEFA_LLVM_UNARY_HPP
