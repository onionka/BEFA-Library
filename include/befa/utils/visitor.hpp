//
// Created by miro on 11/30/16.
//

#ifndef BEFA_VISITOR_HPP
#define BEFA_VISITOR_HPP

#include <typeinfo>

/**
 * Implements visit method for certain type of visitable
 *
 * @param type is derivation from VisitableBase
 * @param visitable is name of variable passed to you
 */
#define IMPLEMENT_VISIT(type, visitable) \
    void visit(const type *visitable) override

/**
 * @param type is type of visitable you are wanna to visit
 * @param visit_impl is method that will be called
 */
#define VISIT_(type, visit_impl) \
    IMPLEMENT_VISIT(type, visitable) \
      { visit_impl(visitable); } \

// ~~~~~ Declarations ~~~~~
template<typename ...Ts>
struct VisitorBase;

template<typename ...Ts>
struct VisitableBase;

template<typename Derived, typename ...Ts>
struct VisitableImpl;
// ~~~~~ Declarations ~~~~~


// ~~~~~ Implementation ~~~~~
/** base for all visitors */
template<typename T>
struct VisitorBase<T> {
  /** visits base instruction (no fucking way) */
  virtual void visit(const T *) {}
};

/** base for all visitors */
template<typename T, typename ...Ts>
struct VisitorBase<T, Ts...> : public VisitorBase<Ts...> {
  /** promote the function(s) from the base class */
  using VisitorBase<Ts...>::visit;

  /** visits base instruction (no fucking way) */
  virtual void visit(const T *) {}
};

template<typename T>
using remove_all_t = std::remove_volatile_t<
    std::remove_pointer_t<
        std::remove_const_t<
            std::remove_reference_t<
                std::remove_all_extents_t<T>
            >
        >
    >
>;

/** Base class for all visitable objects */
template<typename ...Ts>
struct VisitableBase {
  /** Accepts visitor (calls appropriate function to this instruction type) */
  virtual void accept(VisitorBase<Ts...> &) = 0;

  /**
   *
   * @tparam FunctionT
   * @param visitor
   * @return
   */
  template<typename FunctionT>
  VisitableBase<Ts...> &operator>>(FunctionT &&visitor) {
    using lambda_type = remove_all_t<FunctionT>;
    using param_type = typename parse_visitor<
        typename std::remove_pointer<
            decltype(&lambda_type::operator())
        >::type
    >::param_type;
    LambdaVisitor <FunctionT, param_type> lambda(visitor);
    this->accept(lambda);
    return *this;
  }

 private:
  template<typename LambdaT, typename _T>
  struct LambdaVisitor : public VisitorBase<Ts...> {
    LambdaVisitor(LambdaT &lambda)
        : lambda(lambda) {}

    using VisitorBase<Ts...>::visit;

    void visit(const _T *val) override {
      lambda(val);
    }
   private:
    LambdaT &lambda;
  };

  template<typename lambdaT>
  struct parse_visitor {};

  template<typename cls, typename paramT, typename retT>
  struct parse_visitor<retT(cls::*)(paramT)> {
    using param_type = remove_all_t<paramT>;
    using return_type = remove_all_t<retT>;
  };

  template<typename cls, typename paramT, typename retT>
  struct parse_visitor<retT(cls::*)(paramT) const> {
    using param_type = remove_all_t<paramT>;
    using return_type = remove_all_t<retT>;
  };

  template<typename paramT, typename retT>
  struct parse_visitor<retT(*)(paramT)> {
    using param_type = remove_all_t<paramT>;
    using return_type = remove_all_t<retT>;
  };
};

/** Base class for all visitable objects (pre-implemented accept method) */
template<typename Derived, typename ...Ts>
struct VisitableImpl : public VisitableBase<Ts...> {
  /** Accepts visitor (calls appropriate function to this instruction type) */
  void accept(VisitorBase<Ts...> &visitor) override final {
    visitor.visit(static_cast<const Derived *>(this));
  }
};

template<typename Visitable, typename Visitor>
void invoke_accept(Visitable &&visitable, Visitor &&visitor) {
  visitable->accept(visitor);
}

namespace details {
template<typename ...VisitablesT>
struct visitable_traits {
  template<typename DerivedT>
  using visitable_impl = VisitableImpl<DerivedT, VisitablesT...>;

  using visitable_base = VisitableBase<VisitablesT...>;

  using visitor_base = VisitorBase<VisitablesT...>;
};
}
// ~~~~~ Implementation ~~~~~

#endif //BEFA_VISITOR_HPP
