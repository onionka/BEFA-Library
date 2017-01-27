//
// Created by miro on 11/30/16.
//

#ifndef BEFA_VISITOR_HPP
#define BEFA_VISITOR_HPP

#include <typeinfo>
#include <functional>

#include "algorithms.hpp"


/**
 * Implements visit method for certain type of visitable
 *
 * @param type is derivation from VisitableBase
 * @param visitable is name of variable passed to you
 */
#define IMPLEMENT_VISIT(type, visitable) \
    void visit(const type *visitable ATTRIBUTE_UNUSED) override

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

template<typename visitable_traits, typename BaseT, typename ...Ts>
struct Generalizer;

template<typename visitable_traits, typename BaseT, typename ...Ts>
struct LambdaGeneralizer;
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
  /**
   * Adapts lambdas into visitor
   *
   * @tparam LambdaT class of lambda
   * @tparam _T type that lambda accepts as parameter
   */
  template<typename LambdaT, typename _T>
  struct LambdaVisitor : public VisitorBase<Ts...> {
    LambdaVisitor(LambdaT &lambda) : lambda(lambda) {}

    using VisitorBase<Ts...>::visit;

    void visit(const _T *val) override { lambda(val); }
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
struct VisitableImpl : public virtual VisitableBase<Ts...> {
  /** Accepts visitor (calls appropriate function to this instruction type) */
  void accept(VisitorBase<Ts...> &visitor) override {
    visitor.visit(static_cast<const Derived *>(this));
  }
};

// Learn a 'is_pointer' function to know shared and weak ptr
namespace std {
template<typename _Tp>
struct is_pointer<shared_ptr<_Tp>>
    : true_type {};
template<typename _Tp>
struct is_pointer<shared_ptr<_Tp> &>
    : true_type {};
template<typename _Tp>
struct is_pointer<const shared_ptr<_Tp> &>
    : true_type {};
template<typename _Tp>
struct is_pointer<weak_ptr<_Tp>>
    : true_type {};
template<typename _Tp>
struct is_pointer<weak_ptr<_Tp> &>
    : true_type {};
template<typename _Tp>
struct is_pointer<const weak_ptr<_Tp> &>
    : true_type {};
}  // namespace std


template<typename Visitable, typename Visitor>
typename std::enable_if<
    std::is_pointer<Visitable>::value,
    void
>::type invoke_accept(
    Visitable &&visitable,
    Visitor &&visitor
) {
  assert_ex((bool)visitable, "invalid visitable");
  visitable->accept(visitor);
}

template<typename Visitable, typename Visitor>
typename std::enable_if<
    (!std::is_pointer<Visitable>::value),
    void
>::type invoke_accept(
    Visitable &&visitable,
    Visitor &&visitor
) { visitable.accept(visitor); }


template<typename ...VisitablesT>
struct visitable_traits {
  template<typename DerivedT>
  using visitable_impl = VisitableImpl<DerivedT, VisitablesT...>;

  using visitable_base = VisitableBase<VisitablesT...>;

  using visitor_base = VisitorBase<VisitablesT...>;
};

/**
 * In visitor pattern, you can only walk on leafs
 * but the generalizer allows you visit bases
 *
 * @tparam BaseT is base type to which you want to generalize
 * @tparam Ts are types of leafs to visit as BaseT
 */
template<typename visitable_traits, typename BaseT, typename ...Ts>
struct Generalizer
    : visitable_traits::visitor_base {
  using base_type = BaseT;

  /**
   * By implementing this, you can visit object's base class
   * not only leaf classes
   *
   * ie.
   * let classes: A, B, AB = inherits(A,B)
   *
   *
   */
  virtual void generalized_visitor(const BaseT *) = 0;
};

/**
 * Recursivelly implements visits that are calling the same function
 *
 * @see generalized_visitor
 */
template<typename visitable_traits, typename BaseT, typename T, typename ...Ts>
struct Generalizer<visitable_traits, BaseT, T, Ts...>
    : Generalizer<visitable_traits, BaseT, Ts...> {

  /**
   * Implementation of visit for each of derivations
   */
  void visit(const T *visitable) override {
    this->generalized_visitor((const BaseT *)(visitable));
  }
};

template<typename visitable_traits, typename BaseT, typename ...Ts>
struct LambdaGeneralizer final
    : public Generalizer<visitable_traits, BaseT, Ts...> {

  using Base = Generalizer<visitable_traits, BaseT, Ts...>;

  /**
   * Creates generalizer from lambda
   *
   * @tparam LambdaT class of lambda object
   * @param lambda object
   */
  template<typename LambdaT>
  LambdaGeneralizer(LambdaT &&lambda)
      : lambda(lambda) {}

  /**
   * Calls generalized visitor, implemented in Generalizer
   *
   * @param ptr
   */
  void operator()(const BaseT *ptr) {
    generalized_visitor(ptr);
  }

  /**
   * Generalizer requirement
   *
   * @param ptr
   */
  void generalized_visitor(const BaseT *ptr) override {
    lambda(ptr);
  }

 private:
  std::function<void(const BaseT *)> lambda;
};

// ~~~~~ Implementation ~~~~~

#endif //BEFA_VISITOR_HPP
