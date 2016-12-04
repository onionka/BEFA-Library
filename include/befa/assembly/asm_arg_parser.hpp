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
#include "section.hpp"
#include "symbol.hpp"

namespace symbol_table {
/**
 * Types of symbols
 */
namespace types {
using BIT = int8_t;
using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;
struct XMM { QWORD lower:64, higher:64; };

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
  STATIC_CONST size_t size = 1/*bit_sizeof(BIT)*/;
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

#undef STATIC_CONST
}  // namespace types

struct Symbol;

struct Temporary;

template<typename>
struct SizedTemporary;

template<typename>
struct Register;

struct Function;

struct Immidiate;

namespace details {
#define VISITABLE_LIST \
    Temporary, Function, Immidiate, \
    Register<types::BIT>, Register<types::BYTE>, Register<types::WORD>, \
    Register<types::DWORD>, Register<types::QWORD>, Register<types::XMM>, \
    SizedTemporary<types::BYTE>, SizedTemporary<types::WORD>, \
    SizedTemporary<types::DWORD>, SizedTemporary<types::QWORD>, \
    SizedTemporary<types::XMM>

#define IMPLEMENT_VISIT(type, visitable) \
    void visit(const type *visitable)

#define VISIT_ALL(visitable) \
    IMPLEMENT_VISIT(symbol_table::Temporary, visitable) \
      { __visit_impl(visitable); } \
    IMPLEMENT_VISIT(symbol_table::Function, visitable) \
      { __visit_impl(visitable); } \
    IMPLEMENT_VISIT(symbol_table::Immidiate, visitable) \
      { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::Register<symbol_table::types::BIT>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::Register<symbol_table::types::BYTE>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::Register<symbol_table::types::WORD>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::Register<symbol_table::types::DWORD>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::Register<symbol_table::types::QWORD>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::Register<symbol_table::types::XMM>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::SizedTemporary<symbol_table::types::BYTE>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::SizedTemporary<symbol_table::types::WORD>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::SizedTemporary<symbol_table::types::DWORD>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::SizedTemporary<symbol_table::types::QWORD>, visitable \
    ) { __visit_impl(visitable); } \
    IMPLEMENT_VISIT( \
      symbol_table::SizedTemporary<symbol_table::types::XMM>, visitable \
    ) { __visit_impl(visitable); } \
    private: template<typename T> void __visit_impl(const T *visitable)

using visitor_traits = ::details::visitable_traits<VISITABLE_LIST>;

}
template<typename DerivedT>
using Visitable = details::visitor_traits::visitable_impl<DerivedT>;
using VisitableBase = details::visitor_traits::visitable_base;
using VisitorBase = details::visitor_traits::visitor_base;

struct Symbol {
  using symbol_ptr = std::shared_ptr<VisitableBase>;
  using temporary_ptr = std::shared_ptr<VisitableBase>;

  /**
   * @return name of this symbol (like eax or ebp)
   */
  virtual std::string getName() const = 0;

  /**
   * Creates Temporary based on operation (like add or substract)
   *
   * @param lhs
   * @param op
   * @param rhs
   * @return
   */
  static temporary_ptr perform_operation(
      symbol_ptr lhs,
      std::string op,
      symbol_ptr rhs
  );

  /**
   * Creates Temporary based on unary operation
   *
   * @param lhs
   * @param op
   * @param rhs
   * @return
   */
  static temporary_ptr perform_operation(
      std::string op,
      symbol_ptr rhs
  );
};

/**
 * Static symbols in symbol table
 */
extern const std::map<std::string, VisitableBase *> registers;
constexpr auto register_deleter = [](const VisitableBase *) throw() {};

/**
 * Symbol that has size
 *
 * @tparam SizeT
 */
template<typename SizeT>
struct SizedSymbol {
  using size_trait = types::type_trait<SizeT>;
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

  std::string getName() const override {
    NameVisitor name_visitor;
    std::string lhs_name, rhs_name;
    name_visitor.bind(&lhs_name);
    if (getLeft())
      getLeft()->accept(name_visitor);
    else
      lhs_name = "";

    name_visitor.bind(&rhs_name);
    if (getRight())
      getRight()->accept(name_visitor);
    else
      rhs_name = "";

    // if it is unary () should be used
    if (lhs_name == "")
      rhs_name = "(" + rhs_name + ")";
    return lhs_name + getOperator() + rhs_name;
  }
  // ~~~~~ Getters ~~~~~

 private:
  struct NameVisitor
      : public VisitorBase {
    void bind(std::string *output) {
      this->output = output;
    }

    VISIT_ALL(sym) {
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
    : public Temporary, public SizedSymbol<SizeT> {
  SizedTemporary(
      const symbol_ptr &lhs,
      const std::string &op,
      const symbol_ptr &rhs
  ) : Temporary(lhs, op, rhs) {}

  SizedTemporary(
      const std::string &op,
      const symbol_ptr &rhs
  ) : Temporary(op, rhs) {}
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
      public Symbol {
  static constexpr size_t bit_size = bit_sizeof(size);
  constexpr Register(std::string name) : name(name) {}
  std::string getName() const override { return name; }
 private:
  std::string name;
};

/**
 * If instruction is a call, this is address that is being jumped to
 */
struct Function
    : public Visitable<Function>,
      public Symbol {
  using function_ptr = befa::Symbol<befa::Section>;

  Function(const function_ptr &callee)
      : callee(callee) {}

  std::string getName() const override {
    return "@" + callee.getName() + "()";
  }

 private:
  function_ptr callee;
};

/**
 * This is most probably a number
 */
struct Immidiate
    : public Visitable<Immidiate>,
      public Symbol {
  Immidiate(std::string value)
      : value(std::move(value)) {}

  std::string getName() const override {
    return value;
  }

  /**
   * @return value of this immidiate
   */
  const std::string &getValue() const {
    return value;
  }
 private:
  std::string value;
};
}

// ~~~~~ Standard Hash and EqualTo overloads for Symbol ~~~~~
namespace std {
/**
 * Our hash for symbols
 */
template<>
struct hash<std::shared_ptr<symbol_table::Symbol>> {
  size_t operator()(const std::shared_ptr<symbol_table::Symbol> &s) const {
    return std::hash<std::string>()(s->getName());
  }
};

template<>
struct equal_to<std::shared_ptr<symbol_table::Symbol>>
    : public binary_function<
        std::shared_ptr<symbol_table::Symbol>,
        std::shared_ptr<symbol_table::Symbol>,
        bool> {
  bool operator()(
      const std::shared_ptr<symbol_table::Symbol> &__x,
      const std::shared_ptr<symbol_table::Symbol> &__y
  ) const { return __x->getName() == __y->getName(); }
};
}  // namespace std
// ~~~~~ Standard Hash and EqualTo overloads for Symbol ~~~~~

struct SymbolTable {
  using symbol_ptr = std::shared_ptr<symbol_table::Symbol>;
  using symbol_table_type = std::unordered_set<symbol_ptr>;

  /**
   * Inserts symbol into the table
   *
   * @param sym symbol shared pointer
   */
  void addSymbol(const symbol_ptr &sym) {
    assert(table.find(sym) != table.cend() && "symbol already exists");
    table.insert(sym);
  }

  /**
   * Find symbol in symbol table by its name
   * @param name
   * @return
   */
  symbol_ptr find(const std::string &name) {
    auto sym_ite = std::find_if(
        table.cbegin(), table.cend(),
        [&name](const symbol_ptr &sym) {
          return sym->getName() == name;
        }
    );
    if (sym_ite != table.cend()) return *sym_ite;
    else return nullptr;
  }

  /**
   * @return table of symbols
   */
  const symbol_table_type &getTable() const {
    return table;
  }

 private:
  symbol_table_type table;
};

struct asm_arg_parser {
  using instruction_pieces = std::vector<std::string>;
  using symbol_ptr = std::shared_ptr<symbol_table::VisitableBase>;

  std::vector<symbol_ptr> getArgs() {
    instruction_pieces arr = parse();
    assert(!arr.empty() && "instruction pieces cannot be empty");
    std::string name = arr[0];
    std::vector<symbol_ptr> params;

    for (auto ite = arr.begin() + 1,
             end = arr.end();
         ite != end;
         ++ite) {
      if (auto expr = handle_expression(*ite)) {
        params.emplace_back(std::move(expr));
      } else {
        name += " " + *ite;
      }
    }
    // so we could know which params were not parsed
    name = "(" + name + ")";
    return std::move(params);
  }

  virtual instruction_pieces parse() const = 0;

 private:

  struct VarVisitor : public symbol_table::VisitorBase {
#define DEFINE_REG_VISITOR(size) \
    void visit(const symbol_table::Register<size> *reg) { \
      printf("%s with size %lu\n", reg->getName().c_str(), reg->bit_size); \
    }

    DEFINE_REG_VISITOR(symbol_table::types::BIT)
    DEFINE_REG_VISITOR(symbol_table::types::BYTE)
    DEFINE_REG_VISITOR(symbol_table::types::WORD)
    DEFINE_REG_VISITOR(symbol_table::types::DWORD)
    DEFINE_REG_VISITOR(symbol_table::types::QWORD)

#undef DEFINE_REG_VISITOR
  };

  /**
   * This just parses argument, then returns newly created Symbol
   * @param expr
   * @return
   */
  std::shared_ptr<symbol_table::VisitableBase> handle_expression(
      std::string expr
  ) {
    { // if (possible) parameter is register
      auto reg_symbol = symbol_table::registers.find(expr);
      if (reg_symbol != symbol_table::registers.end()) {
//      auto visitor = VarVisitor();
//      params.back()->accept(visitor);
        return std::shared_ptr<symbol_table::VisitableBase>(
            reg_symbol->second, symbol_table::register_deleter
        );
      }
    }

    { // is it value?
      std::smatch result;
      std::regex regex("0x0*([0-9a-fA-F]+)");
      if (std::regex_match(expr, result, regex)) {
        return create_imm(result.str(1));
      }
    }

    { // is it non hex value?
      std::smatch result;
      std::regex regex("([0-9]+)");
      if (std::regex_match(expr, result, regex)) {
        return create_imm(result.str(1));
      }
    }

    { // it is dereference?
      std::smatch result;
      std::regex regex(
          "(XMMWORD|BYTE|WORD|DWORD|QWORD) PTR (?:\\w+:)?\\[(.*)\\]"
      );
      // dereference
      if (std::regex_match(expr, result, regex)) {
        return create_dereference(
            result.str(1), result.str(2)
        );
      }
    }

    { // is it operation?
      std::smatch result;
      std::regex regex("(.+)(\\*|\\+|\\-)(.+)");
      if (std::regex_match(expr, result, regex)) {
        return create_operation(result.str(1), result.str(2), result.str(3));
      }
    }

    return nullptr;
  }

  std::shared_ptr<symbol_table::VisitableBase> create_imm(
      std::string value
  ) {
    return std::make_shared<symbol_table::Immidiate>(value);
  }

  std::shared_ptr<symbol_table::VisitableBase> create_operation(
      std::string lhs,
      std::string op,
      std::string rhs
  ) {
    if (auto lhs_expr = handle_expression(lhs))
      if (auto rhs_expr = handle_expression(rhs))
        return std::make_shared<symbol_table::Temporary>(
            lhs_expr, op, rhs_expr
        );
    throw std::runtime_error(
        "'" + lhs + op + rhs + "' expression not implemented"
    );
  }

  std::shared_ptr<symbol_table::VisitableBase> create_dereference(
      std::string size, std::string expr
  ) {
    using namespace symbol_table::types;

    using temp_byte = symbol_table::SizedTemporary<BYTE>;
    using temp_word = symbol_table::SizedTemporary<WORD>;
    using temp_dword = symbol_table::SizedTemporary<DWORD>;
    using temp_qword = symbol_table::SizedTemporary<QWORD>;
    using temp_xmm = symbol_table::SizedTemporary<XMM>;

#define IMPLEMENT_TYPE_HANDLER(type) do {\
  if (size == type::size_trait::name) { \
    if (auto rhs = handle_expression(expr)) \
      return std::make_shared<type>("*", rhs); \
    else {}\
      /*throw std::runtime_error(expr + " not implemented"); */\
  } \
} while (false)

    IMPLEMENT_TYPE_HANDLER(temp_byte);
    IMPLEMENT_TYPE_HANDLER(temp_word);
    IMPLEMENT_TYPE_HANDLER(temp_dword);
    IMPLEMENT_TYPE_HANDLER(temp_qword);
    IMPLEMENT_TYPE_HANDLER(temp_xmm);

#undef IMPLEMENT_TYPE_HANDLER

    throw std::runtime_error(
        "unknown type of parameter SIZE: " + size + " EXPR: " + expr);
  }

};

#endif //BEFA_INSTRUCTION_DECODER_HPP
