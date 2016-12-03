//
// Created by miro on 12/2/16.
//

#ifndef BEFA_INSTRUCTION_DECODER_HPP
#define BEFA_INSTRUCTION_DECODER_HPP

#include <unordered_set>
#include <map>
#include <string>

#include "../utils/visitor.hpp"
#include "section.hpp"
#include "symbol.hpp"

namespace symbol_table {
/**
 * Types of symbols
 */
namespace types {
using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;

#define STATIC_CONST static constexpr

template<typename SizeT>
struct type_trait {};
template<>
struct type_trait<BYTE> {
  STATIC_CONST const char *name = "BYTE";
  STATIC_CONST size_t size = sizeof(BYTE);
};
template<>
struct type_trait<WORD> {
  STATIC_CONST const char *name = "WORD";
  STATIC_CONST size_t size = sizeof(WORD);
};
template<>
struct type_trait<DWORD> {
  STATIC_CONST const char *name = "DWORD";
  STATIC_CONST size_t size = sizeof(DWORD);
};
template<>
struct type_trait<QWORD> {
  STATIC_CONST const char *name = "QWORD";
  STATIC_CONST size_t size = sizeof(QWORD);
};

#undef STATIC_CONST
}  // namespace types

struct Symbol;

struct Temporary;

template<typename size_t>
struct Register;

struct Function;

struct Immidiate;

namespace details {
using visitor_traits = ::details::visitable_traits<
    Symbol, Temporary, Function, Immidiate,
    Register<uint8_t>, Register<uint16_t>,
    Register<uint32_t>, Register<uint64_t>
>;
}
template<typename DerivedT>
using Visitable = details::visitor_traits::visitable_impl<DerivedT>;
using VisitableBase = details::visitor_traits::visitable_base;
using VisitorBase = details::visitor_traits::visitor_base;

struct Symbol {
  using symbol_ptr = std::shared_ptr<Symbol>;
  using temporary_ptr = std::shared_ptr<Symbol>;

  /**
   * @return name of this symbol (like eax or ebp)
   */
  virtual std::string getName() const = 0;

  /**
   * Creates Temporary based on operation (like add or substract)
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
 * Result of some kind of operation
 */
struct Temporary
    : public Visitable<Temporary>,
      public Symbol {
  using symbol_ptr = std::shared_ptr<Symbol>;

  Temporary(symbol_ptr lhs, std::string op, symbol_ptr rhs)
      : lhs(lhs), op(op), rhs(rhs) {}

  Temporary(std::string op, symbol_ptr rhs)
      : lhs(nullptr), op(op), rhs(rhs) {}

  // ~~~~~ Getters ~~~~~
  const symbol_ptr &getLeft() const { return lhs; }

  const std::string &getOperator() const { return op; }

  const symbol_ptr &getRight() const { return rhs; }

  std::string getName() const override {
    auto left = getLeft();
    return (left ? left->getName() : "")
        + getOperator() + getRight()->getName();
  }
  // ~~~~~ Getters ~~~~~

 private:
  symbol_ptr lhs;
  std::string op;
  symbol_ptr rhs;
};

/**
 * @usage
 *  al = Register<uint8_t>
 *  ax = Register<uint16_t>
 *  eax = Register<uint32_t>
 *  rax = Register<uint64_t>
 * @tparam size is type, which `sizeof(size)` is size of this register
 */
template<typename size>
struct Register
    : public Visitable<Register<size>>,
      public Symbol {
  const static size_t byte_size = sizeof(size);

  Register(std::string name)
      : name(name) {}

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

struct instruction_decoder {
  using instruction_pieces = std::vector<std::string>;

  void getArgs(std::shared_ptr<SymbolTable> sym_table_ref) {
    instruction_pieces arr = parse();
    assert(!arr.empty() && "instruction pieces cannot be empty");
    std::string name = arr[0];
    for (auto ite = arr.begin() + 1,
             end = arr.end();
         ite != end;
         ++ite) {
      printf("%s\n", ite->c_str());
    }
  }

  virtual instruction_pieces parse() const = 0;

 private:
  std::shared_ptr<SymbolTable> sym_table_ref;
};

#endif //BEFA_INSTRUCTION_DECODER_HPP
