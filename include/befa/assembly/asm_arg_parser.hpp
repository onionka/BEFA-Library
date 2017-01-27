//
// Created by miro on 12/2/16.
//

#ifndef BEFA_INSTRUCTION_DECODER_HPP
#define BEFA_INSTRUCTION_DECODER_HPP

#include <unordered_set>
#include <map>
#include <string>
#include <regex>

#include "../utils/assert.hpp"
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

// ~~~~~ __is_integral implementation ~~~~~
namespace details {
template<typename T>
struct __is_integral : std::false_type {};

template<>
struct __is_integral<BYTE> : std::true_type {};
template<>
struct __is_integral<WORD> : std::true_type {};
template<>
struct __is_integral<DWORD> : std::true_type {};
template<>
struct __is_integral<QWORD> : std::true_type {};
}  // namespace details

template<typename ...>
struct is_integral : std::true_type {};

template<typename T, typename ...Ts>
struct is_integral<T, Ts...> : std::__and_<
    details::__is_integral<
        typename std::remove_all_extents_t<T>::type
    >,
    is_integral<Ts...>
>::type {
};
// ~~~~~ __is_integral implementation ~~~~~

// ~~~~~ __is_sse implementation ~~~~~
namespace details {
template<typename T>
struct __is_sse : std::false_type {};

template<>
struct __is_sse<XMM> : std::true_type {};
template<>
struct __is_sse<ZMM> : std::true_type {};
template<>
struct __is_sse<YMM> : std::true_type {};
}  // namespace details

template<typename ...>
struct is_sse : std::true_type {};

template<typename T, typename ...Ts>
struct is_sse<T, Ts...> : std::__and_<
    details::__is_sse<
        typename std::remove_all_extents_t<T>::type
    >,
    is_sse<Ts...>
>::type {
};
// ~~~~~ __is_sse implementation ~~~~~

// ~~~~~ __is_bit implementation ~~~~~
namespace details {
template<typename T>
struct __is_bit : std::false_type {};

template<>
struct __is_bit<BIT> : std::true_type {};
}  // namespace details

template<typename ...>
struct is_bit : std::true_type {};

template<typename T, typename ...Ts>
struct is_bit<T, Ts...> : std::__and_<
    details::__is_bit<
        typename std::remove_all_extents_t<T>::type
    >,
    is_bit<Ts...>
>::type {
};
// ~~~~~ __is_bit implementation ~~~~~

#undef STATIC_CONST
}  // namespace types

// ~~~~~ Declaration of Visitables ~~~~~
struct Symbol;

struct Temporary;

template<typename>
struct SizedTemporary;

template<typename>
struct SizedSymbol;

template<typename>
struct Register;

struct Function;

struct Immidiate;

struct Variable;

struct RegisterBase;
// ~~~~~ Declaration of Visitables ~~~~~

// ~~~~~ Mappings ~~~~~
#define ASM_REGISTER_LIST \
  symbol_table::Register<symbol_table::types::BIT>, \
  symbol_table::Register<symbol_table::types::BYTE>, \
  symbol_table::Register<symbol_table::types::WORD>, \
  symbol_table::Register<symbol_table::types::DWORD>, \
  symbol_table::Register<symbol_table::types::QWORD>, \
  symbol_table::Register<symbol_table::types::XMM>

#define ASM_TEMPORARY_LIST \
  symbol_table::SizedTemporary<symbol_table::types::BYTE>, \
  symbol_table::SizedTemporary<symbol_table::types::WORD>, \
  symbol_table::SizedTemporary<symbol_table::types::DWORD>, \
  symbol_table::SizedTemporary<symbol_table::types::QWORD>, \
  symbol_table::SizedTemporary<symbol_table::types::XMM>

#define ASM_SIZED_SYMBOL_LIST \
  symbol_table::SizedSymbol<symbol_table::types::BIT>, \
  symbol_table::SizedSymbol<symbol_table::types::BYTE>, \
  symbol_table::SizedSymbol<symbol_table::types::WORD>, \
  symbol_table::SizedSymbol<symbol_table::types::DWORD>, \
  symbol_table::SizedSymbol<symbol_table::types::QWORD>, \
  symbol_table::SizedSymbol<symbol_table::types::XMM>

#define ASM_SIZED_LIST \
  ASM_REGISTER_LIST, \
  ASM_TEMPORARY_LIST

#define ASM_SYMBOLS_LIST \
  symbol_table::Temporary, \
  symbol_table::RegisterBase, \
  symbol_table::Function, \
  symbol_table::Immidiate, \
  ASM_SIZED_LIST, \
  ASM_SIZED_SYMBOL_LIST

/**
 * List of all visitable classes
 */
#define ASM_VISITABLE_LIST \
  symbol_table::Variable, \
  symbol_table::Symbol, \
  ASM_SYMBOLS_LIST

// ~~~~~ Lambda Visitors ~~~~~
template<typename size_type>
using SizedSymbolVisitorL = LambdaGeneralizer<
    visitable_traits<ASM_VISITABLE_LIST >, symbol_table::SizedSymbol<size_type>,
    ASM_SIZED_LIST
>;

using RegisterVisitorL = LambdaGeneralizer<
    visitable_traits<ASM_VISITABLE_LIST >, symbol_table::RegisterBase,
    ASM_REGISTER_LIST
>;

using TemporaryVisitorL = LambdaGeneralizer<
    visitable_traits<ASM_VISITABLE_LIST >, symbol_table::Temporary,
    ASM_TEMPORARY_LIST
>;

using SymbolVisitorL = LambdaGeneralizer<
    visitable_traits<ASM_VISITABLE_LIST >, symbol_table::Symbol,
    ASM_SYMBOLS_LIST
>;
// ~~~~~ Lambda Visitors ~~~~~

// ~~~~~ Generalized Visitors ~~~~~
template<typename size_type>
using SizedSymbolVisitor = typename SizedSymbolVisitorL<size_type>::Base;
using RegisterVisitor = typename RegisterVisitorL::Base;
using TemporaryVisitor = typename TemporaryVisitorL::Base;
using SymbolVisitor = typename SymbolVisitorL::Base;
// ~~~~~ Generalized Visitors ~~~~~

// ~~~~~ Mappings ~~~~~

using VisitorTraits = visitable_traits<ASM_VISITABLE_LIST >;

template<typename DerivedT>
using Visitable = VisitorTraits::visitable_impl<DerivedT>;
using VisitableBase = VisitorTraits::visitable_base;
using VisitorBase = VisitorTraits::visitor_base;

/**
 * For internal variables - non-assembly
 */
struct Variable : virtual public VisitableBase {
  void accept(VisitorBase &base) override {
    base.visit(this);
  }

  struct Define;

  struct Use;
};


struct Variable::Define
    : public Variable,
      virtual public VisitableBase {
  void accept(VisitorBase &base) override {
    base.visit(this);
  }
};

struct Variable::Use
    : public Variable,
      virtual public VisitableBase{
  void accept(VisitorBase &base) override {
    base.visit(this);
  }
};



/**
 * Base class for all Assembly Symbols
 */
struct Symbol : virtual public VisitableBase {
  using symbol_ptr = std::shared_ptr<VisitableBase>;
  using temporary_ptr = std::shared_ptr<VisitableBase>;

  Symbol(const std::string &name) : name(name) {}

  /**
   * @return name of this symbol (like eax or ebp)
   */
  const std::string &getName() const { return name; }

  void accept(VisitorBase &base) override {
    base.visit(this);
  }

 private:
  std::string name;
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
 * @tparam SizeT is size of this symbol
 * @tparam VisitableBaseT is VisitableBase or class that inherits from it
 */
template<typename SizeT>
struct SizedSymbol
    : private types::type_trait<SizeT>,
      virtual public VisitableBase {
  using size_trait = types::type_trait<SizeT>;
  const std::string type_name = types::type_trait<SizeT>::name;
  const size_t type_size = types::type_trait<SizeT>::size;

  void accept(VisitorBase &base) override {
    base.visit(this);
  }
};

namespace details {
/**
 * @see cast_symbol_to_visitable
 */
template<bool condition, typename BaseT>
struct cast_helper {
  static_assert(
      condition,
      "cannot convert '" __STRING(BaseT) "' into Visitable"
  );
};

template<typename BaseT>
struct cast_helper<true, BaseT> {
  using visitable_type = typename std::conditional_t<
      std::is_pointer<
          typename std::remove_reference_t<BaseT>::type
      >::value,
      VisitableBase *,
      VisitableBase &
  >;
  static visitable_type __cast(BaseT &val) throw() {
    return static_cast<visitable_type>(val);
  }
};
}  // namespace details

/**
 * No throw cast
 * @tparam condition
 * @tparam BaseT
 * @tparam VisitableT
 * @param val
 */
template<bool condition, typename BaseT>
auto cast_symbol_to_visitable(BaseT &&obj) {
  return details::cast_helper<condition, BaseT>(obj);
}

/**
 * Result of some kind of operation
 */
struct Temporary
    : virtual public VisitableBase,
      public Symbol {
  using symbol_ptr = std::shared_ptr<VisitableBase>;

  Temporary(symbol_ptr lhs, std::string op, symbol_ptr rhs)
      : Symbol(fetchName(lhs, op, rhs)), lhs(lhs), op(op), rhs(rhs) {}

  Temporary(std::string op, symbol_ptr rhs)
      : Symbol(fetchName(nullptr, op, rhs)), lhs(nullptr), op(op), rhs(rhs) {}

  void accept(VisitorBase &base) override {
    base.visit(this);
  }

  // ~~~~~ Getters ~~~~~
  const symbol_ptr &getLeft() const { return lhs; }

  const std::string &getOperator() const { return op; }

  const symbol_ptr &getRight() const { return rhs; }

  /**
   * When 'lhs' is not bound, this is unary operator
   * @return
   */
  bool isUnary() const { return !(bool) getLeft(); }
  // ~~~~~ Getters ~~~~~

 private:

  std::string fetchName(
      symbol_ptr lhs, std::string op, symbol_ptr rhs
  ) const;

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
    : public SizedSymbol<SizeT>,
      public Temporary,
      virtual public VisitableBase {
  using size_trait = typename SizedSymbol<SizeT>::size_trait;

  using symbol_ptr = typename Temporary::symbol_ptr;

  SizedTemporary(
      const symbol_ptr &lhs,
      const std::string &op,
      const symbol_ptr &rhs
  ) : Temporary(lhs, op, rhs) {}

  SizedTemporary(
      const std::string &op,
      const symbol_ptr &rhs
  ) : Temporary(op, rhs) {}

  void accept(VisitorBase &base) override {
    base.visit(this);
  }

 private:
  std::string fetchName(
      const std::string &op,
      const symbol_ptr &rhs,
      const symbol_ptr &lhs = nullptr
  ) const {
    return "<" + std::string(size_trait::name) + ">"
        + Temporary::getName();
  }
};

/**
 * So you can visit all registers at once
 */
struct RegisterBase
    : public Symbol,
      virtual public VisitableBase {
  RegisterBase(std::string name) : Symbol(name) {}

  void accept(VisitorBase &base) override {
    base.visit(this);
  }
};

/**
 * Adds size to register
 */
template<typename size>
struct Register
    : public RegisterBase,
      public SizedSymbol<size>,
      virtual public VisitableBase {
  using size_trait = typename SizedSymbol<size>::size_trait;

  Register(std::string name) : RegisterBase(fetch_name(name)) {}

  void accept(VisitorBase &base) override {
    base.visit(this);
  }

 private:
  std::string fetch_name(std::string name) {
    return std::string("<") + size_trait::name + "> " + name;
  }
};

/**
 * If instruction is a call, this is address that is being jumped to
 */
struct Function
    : virtual public VisitableBase,
      public Symbol {
  using asm_symbol_type = befa::Symbol<befa::Section>;
  using asm_symbol_ptr = std::shared_ptr<asm_symbol_type>;

  Function(const asm_symbol_ptr &callee)
      : Symbol(fetchName(callee)), asm_symbol(callee) {}

  asm_symbol_ptr getAsmSymbol() const { return asm_symbol; }

  void accept(VisitorBase &base) override {
    base.visit(this);
  }

 private:
  std::string fetchName(const asm_symbol_ptr &callee) const {
    return "@" + callee->getName();
  }

  asm_symbol_ptr asm_symbol;
};

/**
 * This is most probably a number
 */
struct Immidiate
    : virtual public VisitableBase,
      public Symbol {
  Immidiate(std::string value)
      : Symbol(value), value(value) {}

  /**
   * @return value of this immidiate
   */
  const std::string &getValue() const { return value; }

  void accept(VisitorBase &base) override {
    base.visit(this);
  }
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
   * Expects to return pieces of decoded instruction
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
