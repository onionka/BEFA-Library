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

// ~~~~~~~~~~~ HELPER METHODS ~~~~~~~~~~~
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

namespace {
template<typename _ValueT>
auto ptr_lock_impl(const std::weak_ptr<_ValueT> &ptr) {
  assert(!ptr.expired());
  return ptr.lock();
}
template<typename _ValueT>
auto ptr_lock_impl(std::weak_ptr<_ValueT> &&ptr) {
  assert(!ptr.expired());
  return ptr.lock();
}
}  // namespace

/**
 * Locks weak pointer
 * @param ptr
 * @return
 */
template<typename _WeakPtrT>
auto ptr_lock(_WeakPtrT &&ptr) {
  return ::ptr_lock_impl(std::forward<_WeakPtrT>(ptr));
}
// ~~~~~~~~~~~ HELPER METHODS ~~~~~~~~~~~

#endif //BEFA_ALGORITHMS_HPP
