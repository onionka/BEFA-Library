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

#include "assert.hpp"

// ~~~~~ Vector operations ~~~~~
namespace details {
template<typename _ContainerT>
struct inserter {
  static inline std::back_insert_iterator<_ContainerT>
  back_inserter(_ContainerT &__x) {
    return std::back_inserter(__x);
  }

};

template<typename _KeyT, typename _ValT>
struct inserter<std::map<_KeyT, _ValT>> {
  static inline std::insert_iterator<std::map<_KeyT, _ValT>>
  back_inserter(std::map<_KeyT, _ValT> &__x) {
    return std::inserter(__x, __x.begin());
  }
};
}  // namespace details

/**
 * @brief Known from Typescript .map() function
 * @param arr to be transformed into another array with the same size
 * @param pred converts _T1 item into _T2
 * @return transformed vector
 */
template<
    typename           _IterableT,
    typename           _PredT,
#define NULL_OBJ(cls)   (*(cls *) nullptr)
    typename           _OutputT = std::vector<decltype(NULL_OBJ(_PredT)(*NULL_OBJ(_IterableT).begin()))>
#undef NULL_OBJ
>
_OutputT                map(
    const _IterableT&   arr,
    _PredT &&           pred,
    _OutputT            ret = _OutputT()
) {
  std::transform(       arr.cbegin(),
                        arr.cend(),
     details::inserter<_OutputT>
        ::back_inserter(ret),
                        pred);
  return      std::move(ret);
}

/**
 * @brief The same as std::for_each but passes iterator instead of value
 * @param arr iterable collection
 * @param pred callable object
 */
template<
    typename           _IterableT,
    typename           _PredT
>
void                    for_each(
    const _IterableT&   arr,
    _PredT&&            pred
) {
  for (auto _i = std::cbegin(arr),
           _e = std::cend(arr);
       _i != _e;
       ++_i)
    pred(_i);
}

/**
 * @brief Checks if array contains value via predicate
 * @param arr
 * @param pred
 * @return
 */
template<
    typename           _IterableT,
    typename           _PredT
>
bool                    contains_if(
    const _IterableT&   arr,
    _PredT&&            pred
) {
  return std::find_if(arr.cbegin(), arr.cend(), pred) != arr.cend();
}

/**
 * @brief Checks if array contains value
 * @param arr
 * @param val
 * @return
 */
template<
    typename           _IterableT,
    typename           _ValT =
      typename         _IterableT::value_type>
bool                    contains(
    const _IterableT&   arr,
    _ValT&&             val
) {
  return std::find(arr.cbegin(), arr.cend(), val) != arr.cend();
}

// ~~~~~ Vector operations ~~~~~

// ~~~~~ WEAK_PTR LOCK ~~~~~
namespace details {
template<
    typename           _ValT,
    typename           _PtrT
>
inline auto             handle_wptr(
    _PtrT&&             ptr
)   -> std::shared_ptr<_ValT> {
  const std::weak_ptr <_ValT> &ptr_ref = ptr;
  assert_ex(
      !ptr_ref.expired(),
      "cannot lock pointer of type '" + typeid(_ValT).name() + "'"
  );
#if defined(NASSERT_EX) && NASSERT_EX == 1
  if (!ptr.expired())
    raise std::runtime_error("failed to lock ptr");
#endif
  return ptr_ref.lock();
}

template<
    typename           _ValueT
>
inline auto             ptr_lock_impl(
   const std::weak_ptr<_ValueT>&          ptr
)  -> std::shared_ptr <_ValueT> {
  return handle_wptr  <_ValueT>          (ptr);
}

template<
    typename           _ValueT
>
inline auto             ptr_lock_impl(
    std::weak_ptr     <_ValueT>&&         ptr
) -> std::shared_ptr  <_ValueT> {
  return handle_wptr  <_ValueT>          (ptr);
}
}  // namespace details

/**
 * Locks weak pointer
 * @tparam _WeakPtrT is weak_ptr, but you can't
 *         create universal reference to weak_ptr<T>,
 *         so type is checked in ptr_lock_impl
 * @param ptr
 * @return
 */
template<
    typename           _WeakPtrT
>
inline auto             ptr_lock(
    _WeakPtrT &&        ptr
)   ->decltype(details::ptr_lock_impl(  *(std::remove_reference_t<_WeakPtrT> *) nullptr)) {
  return       details::ptr_lock_impl(    std::forward           <_WeakPtrT   >(ptr    ));
}
// ~~~~~ WEAK_PTR LOCK ~~~~~

// ~~~~~ Type operations ~~~~~

namespace details {
template<typename T>
struct remove_all : std::remove_volatile<
    typename std::remove_pointer<
        typename std::remove_const<
            typename std::remove_reference<
                typename std::remove_all_extents<T>::type
            >::type
        >::type
    >::type
> {};
}  // namespace details

/**
 * @tparam T type from which you want to remove all decorators
 *           like pointers, references, const and similar
 */
template<typename T>
using remove_all_t = typename details::remove_all<T>::type;
// ~~~~~ Type operations ~~~~~

#endif //BEFA_ALGORITHMS_HPP
