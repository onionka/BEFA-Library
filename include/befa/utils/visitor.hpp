//
// Created by miro on 11/30/16.
//

#ifndef BEFA_VISITOR_HPP
#define BEFA_VISITOR_HPP


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
  virtual void visit(const T *) = 0;
};


/** base for all visitors */
template<typename T, typename ...Ts>
struct VisitorBase<T, Ts...> : public VisitorBase<Ts...> {
  /** promote the function(s) from the base class */
  using VisitorBase<Ts...>::visit;

  /** visits base instruction (no fucking way) */
  virtual void visit(const T *) = 0;
};


/** Base class for all visitable objects */
template<typename ...Ts>
struct VisitableBase {
  /** Accepts visitor (calls appropriate function to this instruction type) */
  virtual void accept(VisitorBase<Ts...> &) = 0;
};


/** Base class for all visitable objects (pre-implemented accept method) */
template<typename Derived, typename ...Ts>
struct VisitableImpl : public VisitableBase<Ts...> {
  /** Accepts visitor (calls appropriate function to this instruction type) */
  void accept(VisitorBase<Ts...> &visitor) override final {
    visitor.visit(static_cast<const Derived *>(this));
  }
};

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
