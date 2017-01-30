//
// Created by miro on 11/30/16.
//

#ifndef BEFA_VISITOR_HPP
#define BEFA_VISITOR_HPP

#include <typeinfo>
#include <functional>
#include <memory>

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

namespace details {
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

}  // namespace details

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
    using param_type = typename details::parse_visitor<
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
};

/** Base class for all visitable objects (pre-implemented accept method) */
template<typename Derived, typename ...Ts>
struct VisitableImpl : public virtual VisitableBase<Ts...> {
  /** Accepts visitor (calls appropriate function to this instruction type) */
  void accept(VisitorBase<Ts...> &visitor) override {
    visitor.visit(static_cast<const Derived *>(this));
  }
};

// Learn 'is_pointer' function new pointer types: shared and weak
// added possibility to remove pointer from shared_ptr and unique_ptr
namespace std {
template<typename _Tp>
struct is_pointer<std::shared_ptr<_Tp>> : std::true_type {};
template<typename _Tp>
struct is_pointer<std::shared_ptr<_Tp> &> : std::true_type {};
template<typename _Tp>
struct is_pointer<const std::shared_ptr<_Tp> &> : std::true_type {};
template<typename _Tp>
struct is_pointer<std::weak_ptr<_Tp>> : std::true_type {};
template<typename _Tp>
struct is_pointer<std::weak_ptr<_Tp> &> : std::true_type {};
template<typename _Tp>
struct is_pointer<const std::weak_ptr<_Tp> &> : std::true_type {};
template<typename _Tp>
struct remove_pointer<std::shared_ptr<_Tp>> { using type = _Tp; };
template<typename _Tp>
struct remove_pointer<std::unique_ptr<_Tp>> { using type = _Tp; };
template<typename _Tp>
struct remove_pointer<std::shared_ptr<_Tp> &> { using type = _Tp; };
template<typename _Tp>
struct remove_pointer<std::unique_ptr<_Tp> &> { using type = _Tp; };
template<typename _Tp>
struct remove_pointer<const std::shared_ptr<_Tp> &> { using type = _Tp; };
template<typename _Tp>
struct remove_pointer<const std::unique_ptr<_Tp> &> { using type = _Tp; };
}  // namespace std

namespace details {

template<typename T>
struct decay_ptr_impl {
  constexpr static auto &decay(T ptr) {
    assert_ex(
        (bool) ptr,
        std::string("nullptr dereference of type '")
            + typeid(decltype(*ptr)).name() + "'!"
    );
    return *ptr;
  };
};

template<typename T>
struct decay_nothing {
  constexpr static T decay(T &&ptr) {
    return std::forward<T &>(ptr);
  };
};

template<typename T>
using decay_ptr_base = std::conditional_t<
    std::is_pointer<T>::value,
    decay_ptr_impl<T>,
    decay_nothing<T>
>;

/**
 * Implements method that can turn pointer to reference
 * or just pass the reference (no overhead if macro NASSERT_EX == 1)
 *
 * @tparam T is type
 */
template<typename T>
struct decay_ptr : decay_ptr_base<T> {
  using decay_ptr_base<T>::decay;
};
}  // namespace details

/**
 *
 * @param visitable pointer (or reference to pointer) to visitable
 * @param visitor unique ref to visitor (but not a pointer)
 * @return
 */
template<typename Visitable, typename Visitor>
void invoke_accept(
    Visitable &&visitable,
    Visitor &&visitor
) {
  details::decay_ptr<Visitable>::decay(
      std::forward<Visitable>(visitable)
  ).accept(details::decay_ptr<Visitor &>::decay(
      visitor
  ));
}

namespace details {
template<typename T, bool is_ptr>
struct possible_ptr_impl {
  inline static bool do_check(T ptr) {
    return (bool) ptr;
  }
};

template<typename T>
struct possible_ptr_impl<T, false> {
  inline static bool do_check(T) {
    return true;
  }
};

template<typename T>
using possible_ptr_check = possible_ptr_impl<
    T, std::is_pointer<T>::value
>;

}  // namespace details

/**
 * Works only with lambdas
 * @tparam Visitor
 * @tparam Visitable
 * @tparam Lambda
 * @tparam DefaultValueT
 * @param visitable
 * @param mapper
 * @param default_result
 * @return
 */
template<
    typename Visitor,
    typename Visitable,
    typename Lambda,
    typename DefaultValueT = typename details::parse_visitor<
        typename std::remove_pointer<
            decltype(&Lambda::operator())
        >::type
    >::return_type
>
auto map_visitable(
    Visitable &&visitable,
    Lambda &&mapper,
    DefaultValueT default_result = DefaultValueT()
) -> DefaultValueT {
  if (details::possible_ptr_check<Visitable>::do_check(visitable))
    invoke_accept(visitable, Visitor(
        [&mapper, &default_result](const auto &ptr) {
          default_result = mapper(ptr);
        } // wtf
    ));
  return default_result;
}

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
    this->generalized_visitor((const BaseT *) (visitable));
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
