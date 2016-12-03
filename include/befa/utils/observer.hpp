//
// Created by miro on 16/10/16.
//

#ifndef BEFA_STREAM_HPP
#define BEFA_STREAM_HPP

#include <vector>
#include <forward_list>
#include <functional>
#include <memory>
#include <unistd.h>
#include <execinfo.h>
#include <iostream>
#include <cassert>
#include <type_traits>

template<typename>
struct Observable;

template<typename>
struct Subject;

template<typename>
struct Subscription;

template<typename>
struct Observer;

/** @see /include/befa/utils/algorithms.hpp func_to_address method */
template<typename _FunctionT>
inline uint64_t func_to_address
    (_FunctionT &&func);

/** @see /include/befa/utils/algorithms.hpp ptr_lock method */
template<typename _WeakPtrT>
auto ptr_lock(_WeakPtrT &&ptr);

/**
 * Optional interface for observers
 *
 *  (you can use this, to get access to register_subscription method)
 *
 * @tparam T can be 'const T&'|'T'|'T&&'
 * @tparam SubscriptionT is subscription type (guessed from type T)
 */
template<typename T>
struct Observer {
  using subscription_type = typename Observable<
      typename std::remove_all_extents<T>::type
  >::subscription_type;
  using value_type = T;

  /** All possible optional callbacks */
  virtual void operator()(T) {}

  /**
   * You can register subscription (if needed)
   *
   * <experimental>
   */
  virtual void register_subscription(std::shared_ptr<subscription_type>) {}

  virtual ~Observer() {}
};

/**
 * Subscription provides possibility of unsubscription
 * @tparam ObserverT
 */
template<typename ObserverT>
struct Subscription {
  typedef std::forward_list<ObserverT> list_type;

  Subscription(
      std::shared_ptr<list_type> vector,
      ObserverT observer
  ) : observer(observer), vector(vector) {}

  Subscription() : valid(false) {}

  /**
   * Removes subscription from vector of subscriptions
   *
   * @throws assertion error (lifetime of vector)
   */
  void unsubscribe() {
    assert(valid && "subscription is not valid");
    vector->remove_if([&](const ObserverT &o$) {
      assert(o$ && "cannot stat address of observer");
      return o$ == observer;
    });
  }

  operator bool() {
    return valid;
  }

  // ~~~~~ Move & Copy semantics ~~~~~
  Subscription(const Subscription &cpy)
      : observer(cpy.observer),
        vector(cpy.vector),
        valid(cpy.valid) {}
  Subscription &operator=(const Subscription &cpy) {
    observer = cpy.observer;
    vector = cpy.vector;
    valid = cpy.valid;
    return *this;
  }

  Subscription(Subscription &&cpy)
      : observer(std::move(cpy.observer)),
        vector(std::move(cpy.vector)),
        valid(cpy.valid) {}
  Subscription &operator=(Subscription &&cpy) {
    observer = std::move(cpy.observer);
    vector = std::move(cpy.vector);
    valid = cpy.valid;
    return *this;
  }
  // ~~~~~ Move & Copy semantics ~~~~~
 private:
  ObserverT observer;
  std::shared_ptr<list_type> vector;
  bool valid = true;
};

namespace details {
namespace observable {

template<typename ObserverBase, typename ObserverT>
struct check_observer_type_helper : std::is_base_of<
    ObserverBase, typename std::remove_all_extents<ObserverT>::type
> {
};

template<typename T, typename ObserverT>
using check_observer_type = std::__or_<
    check_observer_type_helper<Observer<T>, ObserverT>,
    check_observer_type_helper<Observer<const T &>, ObserverT>,
    check_observer_type_helper<Observer<T &&>, ObserverT>
>;

}  // namespace observable
}  // namespace details

/**
 * Observable in Reactive style
 *
 * @example
 *     Subject<int> subj;
 *     Observable<int> o$(subj);
 *
 *     auto subscription = o$.subscribe([] (const int &i) {
 *       std::cout << i << std::endl;
 *     });
 *
 *     subj.update(5);
 *
 *     // removes subscription so updates after this
 *     // won't trigger subscription
 *     subscription.unsubscribe();
 *
 *     subj.update(10);
 *
 *  Result:
 *     5
 *
 */
template<typename T>
struct Observable {
  typedef std::function<void(const T &)> callback_type;

  typedef std::shared_ptr<callback_type> callback_ptr;

  typedef std::forward_list<std::shared_ptr<callback_type>> list_type;

  typedef Subscription<std::shared_ptr<callback_type>> subscription_type;

 private:
  struct map_tag {};
  struct conditional_tag {};

  template<typename ObserverT>
  typename std::enable_if<
      (details::observable::check_observer_type<T, ObserverT>::value),
      subscription_type
  >::type subscribe_impl(
      ObserverT &&observer
  ) {
    using callback_type = typename Observable<T>::callback_type;
    auto subscription = std::make_shared<subscription_type>();
    observer.register_subscription(subscription);
    callbacks->emplace_front(
        std::make_shared<callback_type>(std::forward<ObserverT>(observer))
    );
    *subscription = subscription_type(callbacks, callbacks->front());
    return *subscription;
  }

  template<typename ObserverT>
  typename std::enable_if<
      (!details::observable::check_observer_type<T, ObserverT>::value),
      subscription_type
  >::type subscribe_impl(ObserverT &&observer) {
    using callback_type = typename Observable<T>::callback_type;
    callbacks->emplace_front(
        std::make_shared<callback_type>(std::forward<ObserverT>(observer))
    );
    return subscription_type(callbacks, callbacks->front());
  }

 public:
  /**
   * Attaches this into subject by default
   */
  Observable(Subject<T> &subj) { subj.attach(this); }

  // ~~~~~ Move & Copy semantics ~~~~~
  Observable(Observable &&rhs)
      : callbacks(std::move(rhs.callbacks)) {}
  Observable &operator=(Observable &&rhs) {
    callbacks = std::move(rhs.callbacks);
    return *this;
  }

  Observable(const Observable &rhs)
      : callbacks(rhs.callbacks) {}
  Observable &operator=(const Observable &rhs) {
    callbacks = rhs.callbacks;
    return *this;
  }
  // ~~~~~ Move & Copy semantics ~~~~~

  /**
   * Subscribes to the incoming data from Subject<T>
   *
   * @tparam CallbackT
   * @param cllbck
   * @return
   */
  template<typename CallbackT>
  subscription_type subscribe(CallbackT &&cllbck) {
    return subscribe_impl(std::forward<CallbackT>(cllbck));
  }

  /**
   * @example
   *    int i = 0;
   *    Subject<int> number([] { i++; return i; });
   *    auto str_number = number.map([] (const int &num) {
   *        return std::to_string(num);
   *    });
   *    str_number.subscribe([] (const std::string &str) {
   *        std::cout << str << std::endl;
   *    });
   *
   *    number.next();
   *    // prints "1\n"
   *    number.next();
   *    // prints "2\n"
   *    // etc.
   * @tparam T2 Type of mapped value
   * @tparam CallbackT type of conv
   * @param conv function that converts piped value into another
   * @return instance of T2
   */
  template<typename T2, typename CallbackT>
  Observable<T2> map(CallbackT &&conv) {
    return Observable<T2>(*this, conv, typename Observable<T2>::map_tag());
  }

  /**
   * @example
   *    int i = -1;
   *    Subject<int> number([] { i++; return i; });
   *    auto str_number = number.conditional([] (const int &num) {
   *        return num == 1 || num == 2;
   *    });
   *    str_number.subscribe([] (const int &num) {
   *        std::cout << num << std::endl;
   *    });
   *
   *    number.next();
   *    // prints nothing
   *    number.next();
   *    // prints "1\n"
   *    number.next();
   *    // prints "2\n"
   *    number.next();
   *    // prints, again, nothing
   *    // etc.
   * @tparam CallbackT
   * @param conv
   * @return
   */
  template<typename CallbackT>
  Observable conditional(CallbackT &&conv) {
    return Observable(*this, conv, conditional_tag());
  }

  /**
   * Just shortcut to subscribe
   *
   * @tparam ObserverT
   * @param o
   * @return
   */
  template<typename ObserverT>
  subscription_type operator>>(ObserverT &&o) {
    return subscribe(std::forward<ObserverT>(o));
  }

  virtual ~Observable() {}

 protected:
  template<typename>
  friend
  class Subject;

  template<typename>
  friend
  class Observable;

  /** hidden default constructor */
  Observable() {}

  // ~~~~~ Pipe helper functions ~~~~~
  template<typename T2, typename FunctionT>
  Observable(Observable<T2> &rhs, FunctionT &&pred, map_tag) {
    rhs.subscribe([&](const T2 &i) {
      notify(pred(i));
    });
  }

  template<typename FunctionT>
  Observable(Observable &rhs, FunctionT &&pred, conditional_tag) {
    rhs.subscribe([&](const T &i) {
      if (pred(i)) notify(i);
    });
  }
  // ~~~~~ Pipe helper functions ~~~~~


  /**
   * Call all bound callbacks
   *
   * @param val pass the value into callback
   */
  void notify(const T &val) {
    list_type callbacks_backup(*callbacks);
    for (auto &callback : callbacks_backup)
      callback->operator()(val);
  }

 private:
  std::shared_ptr<list_type> callbacks =
      std::make_shared<list_type>();
};

/**
 * Reactive style subject
 * @see Observable
 */
template<typename T>
struct Subject : public Observable<T> {
  typedef Observable<T> observable_type;
  typedef typename observable_type::subscription_type subscription_type;
  typedef std::function<T()> next_function_type;

  Subject() : observable_type() {}

  /**
   * @tparam NextFunctionT type of function parameter 'func'
   * @param func is a function, that returns next number
   */
  template<typename NextFunctionT>
  Subject(NextFunctionT func)
      : next_function(func) {}

  // ~~~~~ Move & Copy semantics ~~~~~
  Subject(Subject &&rhs)
      : observable_type(std::move(rhs)),
        observers(std::move(rhs.observers)),
        next_function(std::move(rhs.next_function)) {}
  Subject &operator=(Subject &&rhs) {
    observable_type::operator=(std::move(rhs));
    observers = std::move(rhs.observers);
    next_function = std::move(rhs.next_function);
    return *this;
  }

  Subject(const Subject &rhs)
      : observable_type(rhs),
        observers(rhs.observers),
        next_function(rhs.next_function) {}
  Subject &operator=(const Subject &rhs) {
    observable_type::operator=(rhs);
    observers = rhs.observers;
    next_function = rhs.next_function;
    return *this;
  }
  // ~~~~~ Move & Copy semantics ~~~~~

  /**
   * @return newly created observable, bound to this subject
   */
  observable_type toObservable() { return observable_type(*this); }

/**
   * @return casted this into observable
   */
  observable_type &asObservable() { return *this; }

  /**
   * Notify all observables (including this subject as observable)
   *
   * @param val
   */
  void update(const T &val) {
    this->notify(val);
    for (auto &o$: *observers) o$->notify(val);
  }

  /**
   * Attach observable - so we can notify them
   *
   * @param o$
   */
  void attach(observable_type *o$) { observers->push_back(o$); }

  /**
   * Executes next function - then pass it into update
   */
  void next() { if (next_function) update(next_function()); }
 private:
  std::shared_ptr<std::vector<observable_type *>> observers =
      std::make_shared<std::vector<observable_type *>>();

  next_function_type next_function;
};

// ~~~~~ Implementations ~~~~~


#endif //BEFA_STREAM_HPP
