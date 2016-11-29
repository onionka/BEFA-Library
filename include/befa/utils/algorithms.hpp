//
// Created by miro on 18/10/16.
//

#ifndef BEFA_ALGORITHMS_HPP
#define BEFA_ALGORITHMS_HPP

#include <algorithm>
#include <assert.h>
#include <vector>
#include <cassert>
#include <memory>

// ~~~~~ Vector operations ~~~~~
/**
 * Known from Typescript .map() function
 * @param arr to be transformed into another array with the same size
 * @param pred converts _T1 item into _T2
 * @return transformed vector
 */
template<typename _IterableT, typename _PredT>
auto map(const _IterableT &arr, _PredT &&pred) {
  std::vector<decltype(pred(arr.back()))> ret;
  std::transform(arr.cbegin(), arr.cend(), std::back_inserter(ret), pred);
  return std::move(ret);
}

/**
 * Known from Typescript .for_each() function
 * @param arr to be transformed into another array with the same size
 * @param pred converts _T1 item into _T2
 * @return transformed vector
 */
template<typename _IterableT, typename _PredT>
void for_each(const _IterableT &arr, _PredT &&pred) {
  for (auto _i = std::cbegin(arr),
           _e = std::cend(arr);
       _i != _e;
       ++_i)
    pred(_i);
}

/**
 * Checks if array contains value via predicate
 * @param arr
 * @param pred
 * @return
 */
template<typename _IterableT, typename _PredT>
bool contains_if(const _IterableT &arr, _PredT &&pred) {
  return std::find_if(arr.cbegin(), arr.cend(), pred) != arr.cend();
}

/**
 * Checks if array contains value
 * @param arr
 * @param val
 * @return
 */
template<typename _IterableT, typename _ValT = typename _IterableT::value_type>
bool contains(const _IterableT &arr, _ValT &&val) {
  return std::find(arr.cbegin(), arr.cend(), val) != arr.cend();
}
// ~~~~~ Vector operations ~~~~~

// ~~~~~ WEAK_PTR LOCK ~~~~~
namespace details {
template<typename _ValueT>
inline auto ptr_lock_impl(const std::weak_ptr<_ValueT> &ptr) {
  assert(!ptr.expired());
  return ptr.lock();
}
template<typename _ValueT>
inline auto ptr_lock_impl(std::weak_ptr<_ValueT> &&ptr) {
  assert(!ptr.expired());
  return ptr.lock();
}
}  // namespace

/**
 * Locks weak pointer
 * @tparam _WeakPtrT is weak_ptr, but you can't
 *         create universal reference to weak_ptr<T>,
 *         so type is checked in ptr_lock_impl
 * @param ptr
 * @return
 */
template<typename _WeakPtrT>
inline auto ptr_lock(_WeakPtrT &&ptr) {
  return ::details::ptr_lock_impl(std::forward<_WeakPtrT>(ptr));
}
// ~~~~~ WEAK_PTR LOCK ~~~~~

// ~~~~~ Function operations ~~~~~
/**
 * Converts std::function into uint64_t number
 * @tparam _MethodT type of func
 * @param func is function object
 * @return address of func
 */

namespace details {

template <typename Function>
struct function_traits
    : public function_traits<decltype(&Function::operator())> {
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> {
  typedef ReturnType (*pointer)(Args...);
  typedef std::function<ReturnType(Args...)> function;
};

template <typename Function>
typename function_traits<Function>::function
to_function (Function & lambda) {
  return static_cast<typename function_traits<Function>::function>(lambda);
}


template<typename _FunctionT>
inline uint64_t func_to_address
    (_FunctionT lambda) {
  auto function = new decltype(to_function(lambda))(
      to_function(lambda)
  );
  void * func = static_cast<void *>(function);
  return (uint64_t) func;
}

//template<typename _FunctionT>
//inline uint64_t func_to_address
//    <std::function<_FunctionT>>(std::function<_FunctionT> func) {
//  auto *func_address = func.template target<_FunctionT *>();
//  assert(func_address && "bad function type");
//  return (uint64_t) (* func_address);
//}
//
//template<typename Class, typename Return, typename ...Arguments>
//uint64_t func_to_address<std::function<Return (Class::*)(Arguments...)>>
//    (std::function<Return (Class::*)(Arguments...)> &executor) {
//  typedef Return (Class::*fnType)(Arguments...);
//  fnType ** fnPointer = executor.template target<fnType *>();
//  if (fnPointer != nullptr) {
//    return (uint64_t) * fnPointer;
//  }
//  return 0;
//}
}

template<typename _FunctionT>
inline uint64_t func_to_address
    (_FunctionT &&func) {
  return details::func_to_address(std::forward<_FunctionT>(func));
}


// ~~~~~ Function operations ~~~~~

#endif //BEFA_ALGORITHMS_HPP
