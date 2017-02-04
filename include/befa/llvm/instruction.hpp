//
// Created by miro on 11/10/16.
//

#ifndef BEFA_LLVM_LLVM_INSTRUCTION_HPP
#define BEFA_LLVM_LLVM_INSTRUCTION_HPP

#include <tuple>
#include <memory>
#include <rxcpp/rx.hpp>

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
#define REGISTER_VISITABLES(                                                  \
    vi, vb, vrb, ...                                                          \
) template <typename                                            DerivedT>     \
  using     vi  = visitable_traits<__VA_ARGS__>::visitable_impl<DerivedT>   ; \
  using     vb  = visitable_traits<__VA_ARGS__>::visitable_base             ; \
  using     vrb = visitable_traits<__VA_ARGS__>::visitor_base               ;

// ~~~~~ Instruction declarations ~~~~~
/**
 * Base of every instruction
 */
struct Instruction;

struct Serializable;

struct TerminatorInstruction;

struct CallInstruction;

//struct InstrictInstruction;

struct CmpInstruction;

struct JumpInstruction;

struct BranchInstruction;

struct Assignment;

struct UnaryInstruction;

struct BinaryOperator;

struct Defines;

struct Uses;

struct Operator;
// ~~~~~ Instruction declarations ~~~~~

// ~~~~~ Endpoint classes
#define LLVM_BRANCH_LIST \
    JumpInstruction, BranchInstruction

#define LLVM_TERMINATOR_LIST \
    LLVM_BRANCH_LIST

#define LLVM_OPERATOR_LIST \
    UnaryInstruction,BinaryOperator

#define LLVM_ASSIGNMENT_LIST \
    LLVM_OPERATOR_LIST
// ~~~~~ Endpoint classes

/**
 * @brief Mid-level base classes (or endpoint)
 */
#define LLVM_INSTRUCTION_LIST \
    CallInstruction, CmpInstruction,   TerminatorInstruction, LLVM_TERMINATOR_LIST, \
    Assignment,      Operator,                                LLVM_OPERATOR_LIST

/**
 * @brief The lowest level of the bases
 */
#define LLVM_VISITABLES \
    Instruction,     Serializable,     Defines,               LLVM_INSTRUCTION_LIST, \
    Uses

// ~~~~~ Generalize lambda-visitors
using                DefinesVisitorL =                        LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, Defines,               LLVM_INSTRUCTION_LIST
>;
using                UsesVisitorL =                           LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, Uses,                  LLVM_INSTRUCTION_LIST
>;
using                BranchVisitorL =                         LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, TerminatorInstruction, LLVM_BRANCH_LIST
>;
using                AssignmentVisitorL =                     LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>,
                                       Assignment,            LLVM_ASSIGNMENT_LIST
>;
using                OperatorVisitorL =                       LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>,
                                       Operator,              LLVM_OPERATOR_LIST
>;
using                VisitorL =                         LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, TerminatorInstruction, LLVM_TERMINATOR_LIST
>;
using                InstructionVisitorL =                    LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, Instruction,           LLVM_INSTRUCTION_LIST
>;
using                SerializableVisitorL =                   LambdaGeneralizer<
    visitable_traits<LLVM_VISITABLES>, Serializable,          LLVM_INSTRUCTION_LIST,
    Serializable
>;
// ~~~~~ Generalize lambda-visitors

// ~~~~~ Generalize visitor base classes
using                BranchVisitor =                 typename BranchVisitorL      ::Base;
using                AssignmentVisitor =             typename AssignmentVisitorL  ::Base;
using                OperatorVisitor =               typename OperatorVisitorL    ::Base;
using                InstructionVisitor =            typename InstructionVisitorL ::Base;
using                SerializableVisitor =           typename SerializableVisitorL::Base;
using                DefinesVisitor =                typename DefinesVisitorL     ::Base;
using                UsesVisitor =                   typename UsesVisitorL        ::Base;
// ~~~~~ Generalize visitor base classes

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
    VisitableImpl,   VisitableBase,     VisitorBase,           LLVM_VISITABLES
)

using                InstructionTraits =visitable_traits      <LLVM_VISITABLES>;

struct           InstructionMapper;
struct           SymTable;
struct           LLVMFactory;

namespace traits {
// ~~~~~ Assembly instruction aliases
using a_ir =     ExecutableFile::inst_t;
using a_bb =     ExecutableFile::bb_t;
using a_sym =    ExecutableFile::sym_t;
using a_sec =    ExecutableFile::sec_t;
// ~~~~~ Assembly instruction aliases

using address =  types::traits ::container <bfd_vma>;

// ~~~~~ LLVM ir aliases
using factory =  types::traits ::container <llvm::LLVMFactory>;
using ir =       types::traits ::container <llvm::VisitableBase>;
using sym_table =types::traits ::container <SymTable>;
using symbol =   types::traits ::container <symbol_table::VisitableBase>;
using sym_map =  types::traits ::container <
      typename   symbol        ::map       <std::string>::shared
>;
// ~~~~~ LLVM ir aliases
}  // namespace traits

struct Serializable
    : virtual public             VisitableBase {
  Serializable(
      std::string                signature
  ) : signature                 (signature) {}

  const std::string&          getSignature() const {
      return                     signature;
  }

  void                           accept(
      VisitorBase&               visitor
  )   const                      override {
    visitor.visit(this);
  }

 private:
  std::string                    signature;
};

/**
 * @brief Container that contains references to the symbols
 *        they can be iterated via rxcpp
 */
struct SymbolContainer {
 private:
  using sym_t =          traits::symbol;
  using sym_table_t =    traits::sym_table;

 public:
  template<typename...           ArgsT>
  SymbolContainer(
      ArgsT&&...                 symbols
  ) : symbols                  ({
             std::forward<ArgsT>(symbols)...
  }) {}

  SymbolContainer(
      sym_t::vector::shared      symbols
  ) : symbols                   (symbols) {}

  SymbolContainer(
      sym_t::ptr::shared         symbols
  ) : symbols                  ({symbols}) {}

  auto                           iterate() const ->
  decltype(rxcpp::sources      ::iterate(sym_t::vector::shared())) {
    return rxcpp::sources      ::iterate(symbols);
  }

 private:
  sym_t::vector::shared          symbols;
};

/**
 * @brief Contains pointers to the symbols
 *        that are used in this instruction
 */
struct Defines
    : virtual public             VisitableBase,
              private            SymbolContainer {
  template<typename...           ArgsT>
  Defines(
      ArgsT&&...                 symbols
  ) : SymbolContainer(
             std::forward<ArgsT>(symbols)...
  ) {}

  void                           accept(
      VisitorBase&               base
  )   const                      override {
    base.visit(this);
  }

  auto                           defines() const ->
  decltype(SymbolContainer::     iterate()) {
    return SymbolContainer::     iterate();
  }
};

/**
 * @brief Contains pointers to the symbols
 *        that are used in this instruction
 */
struct Uses
    : virtual public             VisitableBase,
              private            SymbolContainer {
  template<typename...           ArgsT>
  Uses(
      ArgsT&&...                 symbols
  ) : SymbolContainer(
             std::forward<ArgsT>(symbols)...
  ) {}

  void                           accept(
      VisitorBase&               base
  )   const                      override {
    base.visit(this);
  }

  auto                           uses() const ->
  decltype(SymbolContainer::     iterate()) {
    return SymbolContainer::     iterate();
  }
};

/**
 * Base class for every instruction
 *
 * You can traverse this via Generalizer
 * @see Generalizer
 */
struct Instruction
    : virtual public             Serializable {

  using sym_t =                  traits::          symbol;
  using sym_table_t =            traits::          sym_table;
  using a_ir_t =                 traits::          a_ir;
  using a_bb_t =                 traits::          a_bb;
  using a_sym_t =                traits::          a_sym;
  using a_sec_t =                traits::          a_sec;
  using a_vec_t =                a_ir_t::vector::  value;


  Instruction(
      const a_vec_t&             assembly
  ) : Serializable   (fetch_name(assembly)),
      assembly                  (assembly) {}

  const a_ir_t::vector::value&getAssembly()        const {
    return assembly;
  }

  a_bb_t::ptr::shared         getParent()          const {
    return assembly[0].getParent();
  }

  bfd_vma                     getAddress()         const {
    return assembly[0].getAddress();
  }

  void                           accept(
      VisitorBase&               visitor
  )   const                      override {
    visitor.visit(this);
  }

 protected:
  static inline
  std::string              fetch_name(
      a_ir_t::vector::value      assembly
  ) {
    std::string result = "_asm {\n";
    for (auto &instr : assembly) {
      result += "\t" + instr.getName() + "\n";
    }
    return result + "}\n";
  }

  /**
   * Almost always this will be only one instruction
   * (but occasionally multiple)
   */
  a_ir_t::vector::value          assembly;
};

/**
 * @brief Represents assignment (ie. mov eax, ebx or result of smkind of operation)
 */
struct Assignment
    : public                     Instruction,
      virtual public             Defines
    , virtual public             Uses {
  using sym_t = Instruction::sym_t;

  Assignment(
      const a_vec_t&             assembly,
      sym_t::ptr::shared         target,
      sym_t::ptr::shared         source
  );

  static inline
  std::string              fetch_name(
      sym_t::ptr::shared         target,
      sym_t::ptr::shared         source
  );

  void                           accept(
      VisitorBase&               base
  )   const                      override {
    base.visit(this);
  }
};

/**
 * @brief Just an operator container
 */
struct Operator
    : virtual public             VisitableBase {
  using operator_t             = types::traits::container <std::string>;

  Operator(
      operator_t::type           _operator
  ) : _operator                 (_operator) {}

  operator_t::type             getOperator() const {
    return                     _operator;
  }

  operator_t::type              toString()   const {
    return                       _operator;
  }

  void                            accept(
      VisitorBase&                base
  )   const                       override {
    base.visit(this);
  }

 private:
  operator_t::type               _operator;
};

/**
 * @brief Represents instructions that works only with one
 *        operand
 */
struct UnaryInstruction
    :         public              Assignment,
              public              Operator,
      virtual public              Defines,
      virtual public              Uses {

  /**
   * @brief ie. dereference, call, ...
   * @param assembly
   * @param target
   * @param _operator
   * @param operand
   */
  UnaryInstruction(
      const a_vec_t&             assembly,
      sym_t::ptr::shared         target,
      operator_t::type          _operator,
      sym_t::ptr::shared         operand
  );

  void                          accept(
      VisitorBase&              base
  )   const                     override {
    base.visit(this);
  }

  static inline
  sym_t::ptr::shared      create_result(
      operator_t::type          _operator,
      sym_t::ptr::shared         operand
  );

  static inline
  std::string              fetch_name(
      sym_t::ptr::shared         target,
      operator_t::type          _operator,
      sym_t::ptr::shared         operand
  );
//
//  template<
//      typename                 SizeT
//  >
//  sym_t::ptr::shared       create_result(
//      operator_t ::type         _operator,
//      sym_t::ptr::shared         operand
//  ) {
//    return std::make_shared<
//        symbol_table::SizedTemporary<SizeT>
//    >(_operator, operand);
//  }
};


/**
 * @brief Another virtual base class that adds binary operation behaviour to the instruction
 */
struct BinaryOperator
    : public                     Assignment,
      public                     Operator {
  using bin_op_t               = types::traits::container <std::string>;
  using inst_vect_t            = Instruction::             a_vec_t;

  BinaryOperator(
      const inst_vect_t&         assembly,
      sym_t::ptr::shared         target,
      sym_t::ptr::shared         lhs,
      bin_op_t::type             bin_op,
      sym_t::ptr::shared         rhs
  );

  void                           accept(
      VisitorBase&               base
  )   const                      override {
    base.visit(this);
  }

  static sym_t::ptr::shared      createResult(
         sym_t::ptr::shared      rhs,
         bin_op_t::type          bin_op,
         sym_t::ptr::shared      lhs
 );

 protected:
  static inline
  std::string              fetch_name(
      sym_t::ptr::shared         target,
      sym_t::ptr::shared         lhs,
      bin_op_t::type             bin_op,
      sym_t::ptr::shared         rhs
  );
};

/**
 * These factories are immutable
 * but accumulator is not
 */
struct LLVMFactory {
  using a_ir_t =                 traits::a_ir;
  using ir_t =                   traits::ir;
  using sym_table_t =            traits::sym_table;

  /**
   * Newly arrived instruction will notify every factory
   *
   * @param asm_ir is assembly language instruction as an input of reduction
   * @param sym_table is a symbol table to which this should append new symbols
   *                  or just find them
   * @param llvm_subscriber is a rxcpp subject's subscriber to which this should
   *                        send a newly created instructions (via on_next(...))
   */
  virtual void                   operator()(
      // input instruction
      a_ir_t::c_info::ref        asm_ir,

      // accumulator as a symbol table
      sym_table_t::ptr::shared   sym_table,

      // side-effect output in form of llvm instruction stream
      ir_t::rx::shared_subs      llvm_subscriber
  ) const = 0;
};

/**
 * Symbol table as a result of assembly instruction reduce operation
 */
struct SymTable {
  using sym_t =                  traits::symbol;
  using sym_map_t =              traits::sym_map;

  /**
   * Create mapper with symbol table
   */
  SymTable(
      sym_map_t::ptr::shared     symbol_table
  ) : symbol_table              (symbol_table) {}

  /**
   * Finds symbol by its address
   *
   * @param address in string format
   * @return pointer to VisitableBase or nullptr
   */
  sym_t::ptr::shared             find_symbol(
      const std::string &        name
  )   const;

  /**
   * Finds symbol by its address
   *
   * @param address in string format
   * @return pointer to VisitableBase or nullptr
   */
  sym_t::ptr::shared             find_symbol(
      bfd_vma                    address
  )   const;

  /**
   * @param sym is pointer to symbol
   * @return virtual memory address of symbol
   */
  bfd_vma                        get_address(
      sym_t::ptr::shared         sym
  )   const;

  /**
   *
   * @return const pointer to map (Read Only)
   */
  sym_map_t::ptr::shared         to_map() const;

  // ~~~~~ Mutable operations (can be used as an accumulator in rxcpp reduce)
  template<
      typename                   T,
      typename...                ArgsT
  >
  sym_t::ptr::shared             add_symbol(
      ArgsT&&...                 args
  ) {
    sym_t::ptr::shared symbol = std::make_shared<T>(
        std::forward<ArgsT>(args)...
    );
    std::string name;
    invoke_accept(symbol, symbol_table::SymbolVisitorL(
        [&name] (const symbol_table::Symbol *sym) {
          name = sym->getAddress();
        }
    ));
    assert_ex(
        symbol_table->emplace(std::make_pair(name, symbol)).second,
        "Failed to insert symbol into symbol table"
    );
    return symbol;
  }

  template<
      typename                   T,
      typename...                ArgsT
  >
  sym_t::ptr::shared             get_or_create(
      std::string                address,
      ArgsT&&...                 args
  ) {
    auto ite = symbol_table->find(address);
    if (ite != symbol_table->end())
      return ite->second;
    return add_symbol<T>(std::forward<ArgsT>(args)...);
  }
  // ~~~~~ Mutable operations

 protected:
  /**
   * Mapper-scoped symbol table
   */
  sym_map_t::ptr::shared         symbol_table;
};

/**
 * Base class for a mapper
 *
 * @tparam I type of instruction
 */
struct InstructionMapper {

  /** symbol table traits */
  using sym_table_t =            traits::sym_table;

  /** Assembly instruction traits */
  using a_ir_t =                 traits::a_ir;

  /** Assembly instruction traits */
  using ir_t =                   traits::ir;

  /** Structures that are using factory type */
  using fact_t =                 traits::factory;

  /** Address type == bfd_vma */
  using addr_t =                 traits::address;

  InstructionMapper(
      sym_table_t::ptr::shared   sym_table
  );

  /**
   * This will subscribe into a stream of instruction
   *
   * @param o$ is input observable
   * @return subscription (can be removed)
   * @see rxcpp::flat_map
   */
  sym_table_t::rx::shared_obs    reduce_instr(
      a_ir_t ::rx::obs           o$
  );

  /**
   * Expects that derivations of this will contain observable of instructions
   */
  ir_t::rx::shared_subs          subscriber(
  )   const;

  ir_t::rx::shared_obs           observable(
  )   const;

  /**
   * Adds factory into a list of factories
   *
   * @param ptr is pointer to a factory
   * @see factories
   */
  void register_factory(
      fact_t::ptr::shared        ptr
  );

  /**
   * Removes factory from a list of factories by value
   *
   * @param ptr is pointer to a factory
   * @see factories
   */
  void remove_factory(
      const fact_t::ptr::raw     ptr
  );

 protected:
  /**
   * Copy cons - new factories
   *
   * the rest is just copy referencies
   */
  InstructionMapper(
      const InstructionMapper&   self
  );

 protected:
  sym_table_t::ptr::shared       symbol_table;
  fact_t::vector::shared         factories;
  addr_t::vector::value          traversed_addresses;
  InstructionVisitorL            append_traversed_addr;
  ir_t::rx::shared_subj          created_instructions;
};

#ifndef INSTRUCTION_TEST
# undef REGISTER_VISITABLES
#endif

}  //  namespace llvm

#endif // BEFA_LLVM_LLVM_INSTRUCTION_HPP
