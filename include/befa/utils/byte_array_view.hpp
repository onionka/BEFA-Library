//
// Created by miro on 11/22/16.
//

#ifndef BEFA_BYTE_ARRAY_VIEW_HPP
#define BEFA_BYTE_ARRAY_VIEW_HPP

#include <cassert>
template<typename T>
struct array_view {
  typedef unsigned size_type;
  typedef T item_type;
  typedef item_type *pointer;
  typedef item_type *const_pointer;
  typedef item_type &reference;
  typedef item_type &const_reference;

  // cons
  array_view(const_pointer ptr, size_type size) throw()
      : _p(ptr), _s(size) {}

  // null view
  array_view() = default;

  // Copy & Move cons
  array_view(const array_view &rhs) throw()
      : _p(rhs.get()), _s(rhs.size()) {}

  array_view(array_view &&rhs) throw()
      : _p(rhs.get()), _s(rhs.size()) {}

  // Copy & Move assignment
  array_view &operator=(const array_view &rhs) throw() {
    _p = rhs.get();
    _s = rhs.size();
    return *this;
  }

  array_view &operator=(array_view &&rhs) throw() {
    _p = rhs.get();
    _s = rhs.size();
    return *this;
  }

  const_reference operator[](size_type i) const
#ifdef ARRAY_VIEW_CHECK_BORDERS
  {
    assert(i >= size() && "index out of bound");
#else
    throw() {
#endif
    return _p[i];
  }

  size_type size() const throw() { return _s; }

  const_pointer get() const throw() { return _p; }

  ~array_view() throw() { /* does nothing (memory will be freed by someone else) */ }

 private:
  const_pointer _p = nullptr;
  size_type _s = 0;
};

#endif //BEFA_BYTE_ARRAY_VIEW_HPP
