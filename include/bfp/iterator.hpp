#ifndef __BFP_ITERATOR_HPP
#define __BFP_ITERATOR_HPP


#include <bits/stl_iterator_base_types.h>


template<
    typename __flyweight,
    typename __flyweight_ptr = __flyweight *,
    typename __alloc = typename ::bfp::remove_references<typename __flyweight::__data>::type>
  class Iterator :
      public std::iterator<
          ::std::bidirectional_iterator_tag,
          __flyweight>
    {
  public:
      /** iterator allocator */
      typedef __flyweight flyweight;
      typedef __alloc alloc;

      typedef Iterator<
          __flyweight> ite_type;

      Iterator(
          flyweight *owner,
          size_t offset = 0)
          :
          _data(new alloc()),
          _owner(owner),
          _offset(offset)
        { }

      Iterator(const ite_type &_it)
          :
          _data(_it._data),
          _owner(_it._owner)
        { }

      ite_type &operator++()
        {
          _owner->next(_data);
          return *this;
        }

      ite_type operator++(int)
        {
          return ++(*this);
        }

      ite_type &operator--()
        {
          _owner->prev(_data);
          return *this;
        }

      ite_type operator--(int)
        {
          return --(*this);
        }

      ite_type &operator+(
          size_t _offset)
        {
          for (int i = 0;
               i < _offset;
               i++)
            _owner->next(_data);
          return *this;
        }

      ite_type &operator-(
          size_t _offset)
        {
          for (int i = 0;
               i < _offset;
               i++)
            _owner->prev(_data);
          return *this;
        }

      bool operator==(
          const ite_type &_ite)
        {
          return _offset == _ite._offset;
        }

      bool operator!=(
          const ite_type &_ite)
        {
          return _offset != _ite._offset;
        }

      alloc &operator*()
        {
          return *_data;
        }

      ~Iterator() { }

  private:
      alloc *_data;
      flyweight *_owner;
      size_t _offset;
    };


#endif //__BFP_ITERATOR_HPP
