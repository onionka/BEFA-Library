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
#include "algorithms.hpp"


template<typename>
struct RxObservable;


template<typename>
struct RxSubject;


template<typename>
struct Subscription;


template<typename ObserverT>
struct Subscription {
  typedef std::forward_list<std::shared_ptr<ObserverT>> list_type;

  Subscription(
      const std::weak_ptr<list_type> &vector,
      std::weak_ptr<ObserverT> ptr
  ) : ite(ptr), vector(vector) {}

  void unsubscribe() { ptr_lock(vector)->remove(ptr_lock(ite)); }

  Subscription(const Subscription &cpy)
      : ite(cpy.ite), vector(cpy.vector) {}

  Subscription &operator=(const Subscription &cpy) {
    ite = cpy.ite;
    vector = cpy.vector;
    return *this;
  }

  Subscription(Subscription &&cpy)
      : ite(std::move(cpy.ite)), vector(cpy.vector) {}

  Subscription &operator=(Subscription &&cpy) {
    ite = std::move(cpy.ite);
    vector = cpy.vector;
    return *this;
  }

 private:
  std::weak_ptr<ObserverT> ite;
  std::weak_ptr<list_type> vector;
};

/**
 * Observable in Reactive style
 *
 * @example
 *     RxSubject<int> subj;
 *     RxObservable<int> o$(subj);
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
struct RxObservable {
  typedef std::function<void(const T &)> callback_type;

  typedef std::forward_list<std::shared_ptr<callback_type>> list_type;

  typedef Subscription<callback_type> subscription_type;

  RxObservable(RxSubject<T> &subj) { subj.attach(this); }

  RxObservable(RxObservable &&rhs)
      : callbacks(std::move(rhs.callbacks)) {}
  RxObservable &operator=(RxObservable &&rhs) {
    callbacks = std::move(rhs.callbacks);
    return *this;
  }

  RxObservable(const RxObservable &rhs)
      : callbacks(rhs.callbacks) {}
  RxObservable &operator=(const RxObservable &rhs) {
    callbacks = rhs.callbacks;
    return *this;
  }

  template<typename CallbackT>
  subscription_type subscribe(CallbackT &&cllbck) {
    callbacks->emplace_front(std::make_shared<callback_type>(std::forward<CallbackT>(cllbck)));
    return Subscription<callback_type>(callbacks, std::weak_ptr<callback_type>(callbacks->front()));
  }

 protected:
  template<typename>
  friend class RxSubject;

  RxObservable() {}

  void notify(const T &val) {
    for (auto &callback: *callbacks)
      callback->operator()(val);
  }

 private:
  std::shared_ptr<list_type> callbacks = std::make_shared<list_type>();
};


/**
 * Reactive style subject
 * @see RxObservable
 */
template<typename T>
struct RxSubject : public RxObservable<T> {
  typedef RxObservable<T> observable_type;
  typedef typename observable_type::subscription_type subscription_type;

  RxSubject() : observable_type() { }

  RxSubject(RxSubject &&rhs)
  : observable_type(std::move(rhs)),
    observers(std::move(rhs.observers)) {}
  RxSubject &operator=(RxSubject &&rhs) {
    observable_type::operator=(std::move(rhs));
    observers = std::move(rhs.observers);
    return *this;
  }

  RxSubject(const RxSubject &rhs)
      : observable_type(rhs),
        observers(rhs.observers) {}
  RxSubject &operator=(const RxSubject &rhs) {
    observable_type::operator=(rhs);
    observers = rhs.observers;
    return *this;
  }

  observable_type toObservable() { return observable_type(*this); }
  observable_type &asObservable() { return *this; }

  void update(const T &val) { this->notify(val); for (auto &o$: *observers) o$->notify(val); }

  void attach(observable_type *o$) { observers->push_back(o$); }

 private:
  std::shared_ptr<std::vector<observable_type *>> observers = std::make_shared<std::vector<observable_type *>>();
};


#endif //BEFA_STREAM_HPP
