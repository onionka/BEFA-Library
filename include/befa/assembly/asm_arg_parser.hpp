//
// Created by miro on 12/2/16.
//

#ifndef BEFA_INSTRUCTION_DECODER_HPP
#define BEFA_INSTRUCTION_DECODER_HPP

#include <unordered_set>
#include <map>
#include <string>
#include <regex>

#include "../utils/visitor.hpp"
#include "../utils/bucket_allocator.hpp"
#include "section.hpp"
#include "symbol.hpp"

namespace symbol_table {
/**
 * Types of symbols
 */
namespace types {
struct BIT { uint8_t val:1; };
using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;
struct XMM { QWORD lower:64, higher:64; };

// ~~~~~ NOT IMPLEMENTED TYPES ~~~~~
struct YMM { XMM lower, higher; };
struct ZMM { YMM lower, higher; };
// ~~~~~ NOT IMPLEMENTED TYPES ~~~~~

/**
 * list of known data types
 */
#define ASM_KNOWN_TYPES \
    symbol_table::types::BIT, \
    symbol_table::types::BYTE, \
    symbol_table::types::WORD, \
    symbol_table::types::DWORD, \
    symbol_table::types::QWORD, \
    symbol_table::types::XMM, \
    symbol_table::types::YMM, \
    symbol_table::types::ZMM

/**
 * @return size of type, but in bits, not bytes
 */
#define bit_sizeof(type) (sizeof(type) * 8)

#define STATIC_CONST static constexpr

/**
 * This contains informations about size types
 *
 * @tparam SizeT is certain size type (BIT, BYTE, ...)
 */
template<typename SizeT>
struct type_trait {};
template<>
struct type_trait<BIT> {
  STATIC_CONST const char *name = "BIT";
  /** note that you cannot have (in c++) variable with size 1bit */
  STATIC_CONST size_t size = 1 /*bit_sizeof(BIT)*/;
};
template<>
struct type_trait<BYTE> {
  STATIC_CONST const char *name = "BYTE";
  STATIC_CONST size_t size = bit_sizeof(BYTE);
};
template<>
struct type_trait<WORD> {
  STATIC_CONST const char *name = "WORD";
  STATIC_CONST size_t size = bit_sizeof(WORD);
};
template<>
struct type_trait<DWORD> {
  STATIC_CONST const char *name = "DWORD";
  STATIC_CONST size_t size = bit_sizeof(DWORD);
};
template<>
struct type_trait<QWORD> {
  STATIC_CONST const char *name = "QWORD";
  STATIC_CONST size_t size = bit_sizeof(QWORD);
};
template<>
struct type_trait<XMM> {
  STATIC_CONST const char *name = "XMMWORD";
  STATIC_CONST size_t size = bit_sizeof(XMM);
};
template<>
struct type_trait<YMM> {
  STATIC_CONST const char *name = "YMMWORD";
  STATIC_CONST size_t size = bit_sizeof(YMM);
};
template<>
struct type_trait<ZMM> {
  STATIC_CONST const char *name = "ZMMWORD";
  STATIC_CONST size_t size = bit_sizeof(ZMM);
};

#undef STATIC_CONST
}  // namespace types

// ~~~~~ Declaration of Visitables ~~~~~
struct Symbol;

struct Temporary;

template<typename>
struct SizedTemporary;

template<typename>
struct Register;

struct Function;

struct Immidiate;
// ~~~~~ Declaration of Visitables ~~~~~

namespace details {
/**
 * List of all visitable classes
 */
#define ASM_VISITABLE_LIST \
    Temporary, Function, Immidiate, \
    Register<types::BIT>, Register<types::BYTE>, Register<types::WORD>, \
    Register<types::DWORD>, Register<types::QWORD>, Register<types::XMM>, \
    SizedTemporary<types::BYTE>, SizedTemporary<types::WORD>, \
    SizedTemporary<types::DWORD>, SizedTemporary<types::QWORD>, \
    SizedTemporary<types::XMM>

/**
 * Visit all registers
 *
 * @param visitable is name of parameter passed to you
 */
#define ASM_VISIT_REGISTERS(visitable) \
    VISIT_( \
      symbol_table::Register<symbol_table::types::BIT>, \
      __visit_##visitable##_impl_registers \
    ) \
    VISIT_( \
      symbol_table::Register<symbol_table::types::BYTE>, \
      __visit_##visitable##_impl_registers \
    ) \
    VISIT_( \
      symbol_table::Register<symbol_table::types::WORD>, \
      __visit_##visitable##_impl_registers \
    ) \
    VISIT_( \
      symbol_table::Register<symbol_table::types::DWORD>, \
      __visit_##visitable##_impl_registers \
    ) \
    VISIT_( \
      symbol_table::Register<symbol_table::types::QWORD>, \
      __visit_##visitable##_impl_registers \
    ) \
    VISIT_( \
      symbol_table::Register<symbol_table::types::XMM>, \
      __visit_##visitable##_impl_registers \
    ) \
    private: template<typename T> \
      void __visit_##visitable##_impl_registers(const T *visitable)

/**
 *
 */
#define ASM_VISIT_SIZED_TEMPORARIES(visitable) \
    VISIT_( \
      symbol_table::SizedTemporary<symbol_table::types::BYTE>, \
      __visit_##visitable##_impl_sized_temp \
    ) \
    VISIT_( \
      symbol_table::SizedTemporary<symbol_table::types::WORD>, \
      __visit_##visitable##_impl_sized_temp \
    ) \
    VISIT_( \
      symbol_table::SizedTemporary<symbol_table::types::DWORD>, \
      __visit_##visitable##_impl_sized_temp \
    ) \
    VISIT_( \
      symbol_table::SizedTemporary<symbol_table::types::QWORD>, \
      __visit_##visitable##_impl_sized_temp \
    ) \
    VISIT_( \
      symbol_table::SizedTemporary<symbol_table::types::XMM>, \
      __visit_##visitable##_impl_sized_temp \
    ) \
    private: template<typename T> \
      void __visit_##visitable##_impl_sized_temp(const T *visitable)

/**
 * Implements all methods to visit temporaries into one
 *
 * @param visitable is name of parameter passed to you
 */
#define ASM_VISIT_TEMPORARIES(visitable) \
    VISIT_( \
      symbol_table::Temporary, \
      __visit_##visitable##_impl_temp \
    ) \
    ASM_VISIT_SIZED_TEMPORARIES(visitable) { \
      __visit_##visitable##_impl_temp(visitable); \
    } \
    private: template<typename T> \
      void __visit_##visitable##_impl_temp(const T *visitable)

/**
 * Implements all visits in one
 *
 * @param visitable is with template type T
 */
#define ASM_VISIT_ALL(visitable) \
    VISIT_(symbol_table::Function, __visit_impl) \
    VISIT_(symbol_table::Immidiate, __visit_impl) \
    ASM_VISIT_REGISTERS(visitable) { __visit_impl(visitable); } \
    ASM_VISIT_TEMPORARIES(visitable) { __visit_impl(visitable); } \
    private: template<typename T> void __visit_impl(const T *visitable)

using visitor_traits = ::details::visitable_traits<ASM_VISITABLE_LIST >;

}
template<typename DerivedT>
using Visitable = details::visitor_traits::visitable_impl<DerivedT>;
using VisitableBase = details::visitor_traits::visitable_base;
using VisitorBase = details::visitor_traits::visitor_base;

/**
 * Base class for all Symbols
 */
struct Symbol {
  using symbol_ptr = std::shared_ptr<VisitableBase>;
  using temporary_ptr = std::shared_ptr<VisitableBase>;

  /**
   * @return name of this symbol (like eax or ebp)
   */
  // std::string getName() const = 0;
};

/**
 * Static symbols in symbol table
 */
extern const std::map<std::string, VisitableBase *> registers;

/**
 * Deleter for static registers
 */
constexpr auto register_deleter = [](const VisitableBase *) throw() {};

/**
 * Symbol that has size
 *
 * @tparam SizeT
 */
template<typename SizeT>
struct SizedSymbol : private types::type_trait<SizeT> {
  using size_trait = types::type_trait<SizeT>;
  const std::string type_name = types::type_trait<SizeT>::name;
  const size_t type_size = types::type_trait<SizeT>::size;
};

/**
 * Result of some kind of operation
 */
struct Temporary
    : public Visitable<Temporary>,
      public Symbol {
  using symbol_ptr = std::shared_ptr<VisitableBase>;

  Temporary(symbol_ptr lhs, std::string op, symbol_ptr rhs)
      : lhs(lhs), op(op), rhs(rhs) {}

  Temporary(std::string op, symbol_ptr rhs)
      : lhs(nullptr), op(op), rhs(rhs) {}

  // ~~~~~ Getters ~~~~~
  const symbol_ptr &getLeft() const { return lhs; }

  const std::string &getOperator() const { return op; }

  const symbol_ptr &getRight() const { return rhs; }

  /**
   * When 'lhs' is not bound, this is unary operator
   * @return
   */
  bool isUnary() const { return !(bool) getLeft(); }

  std::string getName() const;
  // ~~~~~ Getters ~~~~~

 private:
  struct NameVisitor
      : public VisitorBase {
    void bind(std::string *output) {
      this->output = output;
    }

    ASM_VISIT_ALL(sym) {
      assert(output && "output cannot be null");
      if (sym) *output = sym->getName();
    }
   private:
    std::string *output = nullptr;
  };

  symbol_ptr lhs;
  std::string op;
  symbol_ptr rhs;
};

/**
 * Temporary with size
 *
 * @tparam SizeT
 */
template<typename SizeT>
struct SizedTemporary
    : public Temporary,
      public SizedSymbol<SizeT> {
  SizedTemporary(
      const symbol_ptr &lhs,
      const std::string &op,
      const symbol_ptr &rhs
  ) : Temporary(lhs, op, rhs) {}

  SizedTemporary(
      const std::string &op,
      const symbol_ptr &rhs
  ) : Temporary(op, rhs) {}

  std::string getName() const {
    return "<" + std::string(SizedSymbol<SizeT>::size_trait::name) + ">"
        + Temporary::getName();
  }
};

/**
 * @usage
 *  al = Register<uint8_t>
 *  ax = Register<uint16_t>
 *  eax = Register<uint32_t>
 *  rax = Register<uint64_t>
 * @tparam size is type, which `bit_sizeof(size)` is size of this register
 */
template<typename size>
struct Register
    : public Visitable<Register<size>>,
      public Symbol,
      public SizedSymbol<size> {
  constexpr Register(std::string name) : name(name) {}

  std::string getName() const { return name; }

 private:
  std::string name;
};

/**
 * If instruction is a call, this is address that is being jumped to
 */
struct Function
    : public Visitable<Function>,
      public Symbol {
  using function_type = befa::Symbol<befa::Section> ;
  using function_ptr = std::shared_ptr<function_type>;

  Function(const function_ptr &callee) : callee(callee) {}

  std::string getName() const { return "@" + callee->getName(); }

  function_ptr getCallee() const { return callee; }

 private:
  function_ptr callee;
};

/**
 * This is most probably a number
 */
struct Immidiate
    : public Visitable<Immidiate>,
      public Symbol {
  Immidiate(std::string value) : value(std::move(value)) {}

  std::string getName() const { return value; }

  /**
   * @return value of this immidiate
   */
  const std::string &getValue() const { return value; }
 private:
  std::string value;
};
}

struct asm_arg_parser {
  using instruction_pieces = std::vector<std::string>;
  using symbol_ptr = std::shared_ptr<symbol_table::VisitableBase>;
  using symbol_map = std::map<bfd_vma, symbol_ptr>;

  /**
   * Arguments are Immidiate, Expressions, Registers, ...
   *
   * @return vector of parameters
   */
  std::vector<symbol_ptr> getArgs(
      const symbol_map &functions = {}
  ) const throw(std::runtime_error);

  /**
   * Expects to return piecies of decoded instruction
   *
   * @return mov eax,ebx -> ["mov", "eax", "ebx"]
   */
  virtual instruction_pieces parse() const = 0;

 private:

  /**
   * This just parses argument, then returns newly created Symbol
   *
   * @param expr
   * @return Symbol as VisitableBase object
   */
  std::shared_ptr<symbol_table::VisitableBase> handle_expression(
      std::string expr, const symbol_map &functions = {}
  ) const throw(std::runtime_error);

  /**
   * creates immidiate variable
   *
   * @param value immidiate varaible's value
   * @return Immidiate object
   */
  std::shared_ptr<symbol_table::VisitableBase> create_imm(
      std::string value
  ) const throw();

  /**
   * Creates Temporary object
   *
   * @param lhs left hand size expression
   * @param op operation between lsh and rhs
   * @param rhs right hand size expression
   * @return newly created Temporary object with parsed expression
   */
  std::shared_ptr<symbol_table::VisitableBase> create_operation(
      std::string lhs, std::string op, std::string rhs
  ) const throw(std::runtime_error);

  /**
   * Parses expressions like DWORD PTR [<expr>]
   *
   * @param size is size type (DWORD|BYTE|...)
   * @param expr is <expr> that will be parsed later
   * @return newly created SizedTemporary
   */
  std::shared_ptr<symbol_table::VisitableBase> create_dereference(
      std::string size, std::string expr
  ) const throw(std::runtime_error);
};

#endif //BEFA_INSTRUCTION_DECODER_HPP
