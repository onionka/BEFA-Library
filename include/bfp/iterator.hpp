/**
 * @file iterator.hpp
 * @author Miroslav Cibulka
 * @brief Iterators of all kinds
 * @details
 *  example of use:
 *    Let's say we have class that contains raw data and we want to wrap them. We may
 *    use Flyweight pattern to minimalize memory usage and increase speed.
 *    @code
 *      class RawData :
 *        public ::bfp::Vector<
 *            ::bfp::ForwardIterator<
 *                Wrapper,
 *                RawData>>
 *        {
 *        typedef ::bfp::Vector<
 *            ::bfp::ForwardIterator<
 *                Wrapper,
 *                RawData>> _Base;
 *
 *      public:
 *          // Allocation and data fill
 *          ...
 *          /// We need to create begin of iteration
 *          virtual _Base::iterator begin()
 *            {
 *              _Base::iterator _ret(this, 0);
 *              /// that we have to fill it with raw data
 *              return _ret;
 *            }
 *
 *          /// We need to define when iteration should stop
 *          /// This should return iterator with no data
 *          virtual _Base::iterator end()
 *            {
 *              return _Base::iterator(this, size());
 *            }
 *
 *          /// This should fill _wrapper object with data
 *          virtual void next(
 *              _Base::value_type &_wrapper,
 *              _Base::difference_type &_diff)
 *            {
 *              _diff += 1;
 *              _wrapper.data = raw_data + _diff;
 *            }
 *
 *          /// This returns size of data
 *          virtual _Base::difference_type size() { return size_of_data; }
 *
 *          /// This are just aliases to size()
 *          virtual _Base::difference_type capacity() { return size(); }
 *          virtual _Base::difference_type max_size() { return size(); }
 *
 *       private:
 *          /// Raw data
 *          uint8_t *raw_data;
 *
 *          /// Size of raw data
 *          _Base::difference_type size_of_data;
 *        }
 *    @endcode
 *
 *    Requirements:
 *      * Wrapper should be class with default/primitive constructor and RawData
 *        should have access to his attributes.
 *      * Implementation of inherited method
 *
 */

#ifndef __BFP_ITERATOR_HPP
#define __BFP_ITERATOR_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif

#include <bits/stl_iterator_base_types.h>
#include <utility>


namespace bfp
  {
      /** Base class for all vector that use Flyweight iterator
       * @tparam __iterator is expected iterator type (ForwardIterator, BidirectionalIterator ...)
       */
      template<
          typename __iterator>
        class Vector
          {
        public:
            typedef __iterator iterator;
            typedef typename iterator::difference_type difference_type;
            typedef typename iterator::value_type value_type;

            virtual iterator begin() = 0;

            virtual iterator end() = 0;

            virtual void next(
                value_type &,
                difference_type &) = 0;

            virtual void prev(
                value_type &,
                difference_type &)
              { }

            virtual difference_type capacity() = 0;

            virtual difference_type size() = 0;

            virtual difference_type max_size() = 0;

            virtual ~Vector()
              { }
          };

      template<
          typename __wrapper,
          typename __owner,
          typename __tag = ::std::forward_iterator_tag>
        class ForwardIterator :
            public ::std::iterator<
                __tag,
                __wrapper>
          {
        public:
            typedef ::std::iterator<
                __tag,
                __wrapper> _Base;
            typedef __wrapper wrapper;
            typedef __owner owner;
            typedef typename _Base::difference_type difference_type;
            typedef typename _Base::value_type value_type;
            typedef typename _Base::pointer pointer;
            typedef typename _Base::reference reference;
            typedef typename _Base::iterator_category iterator_category;

            typedef ForwardIterator<
                wrapper,
                __owner,
                __tag> ite_type;

            ForwardIterator() = default;

            ForwardIterator(
                owner *own,
                difference_type offset = 0)
                :
                _owner(own),
                _offset(offset)
              { }

            ForwardIterator(
                const ite_type &_cp)
              {
                _data = value_type(_cp._data);
                _owner = _cp._owner;
                _offset = _cp._offset;
              }

            ForwardIterator(
                ite_type &&_mv)
              {
                _data = ::std::move(_mv._data);
                _owner = _mv._owner;
                _offset = _mv._offset;
              }

            ite_type &operator=(
                const ite_type &_cp)
              {
                _data = value_type(_cp._data);
                _owner = _cp._owner;
                _offset = _cp._offset;
                return *this;
              }

            ite_type &operator=(
                ite_type &&_mv)
              {
                _data = ::std::move(_mv._data);
                _owner = _mv._owner;
                _offset = _mv._offset;
                return *this;
              }

            ite_type &operator++()
              {
                _owner->next(_data, _offset);
                return *this;
              }

            ite_type operator++(int)
              {
                ite_type tmp(*this);
                ++*this;
                return tmp;
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

            value_type &operator*()
              {
                return _data;
              }

            value_type *operator->()
              {
                return &_data;
              }

            void swap(ite_type &other)
              {
                ::std::swap(other._data, _data);
                ::std::swap(other._owner, _owner);
                ::std::swap(other._offset, _offset);
              }

            ~ForwardIterator() noexcept
              { }
        private:
            owner *_owner = nullptr;

        protected:
            value_type _data;
            difference_type _offset = -1;
          };

      template<
          typename __wrapper,
          typename __owner,
          typename __tag = ::std::bidirectional_iterator_tag>
        class BidirectionalIterator :
            public ForwardIterator<
                __wrapper,
                __owner,
                __tag>
          {
        public:
            typedef ::std::iterator<
                __tag,
                __wrapper> _Base;
            typedef BidirectionalIterator<
                __wrapper,
                __owner,
                __tag> ite_type;
            typedef __wrapper wrapper;
            typedef __owner owner;
            typedef typename _Base::difference_type difference_type;
            typedef typename _Base::value_type value_type;
            typedef typename _Base::pointer pointer;
            typedef typename _Base::reference reference;
            typedef typename _Base::iterator_category iterator_category;

            BidirectionalIterator() = default;

            BidirectionalIterator(
                owner *own,
                difference_type offset = 0)
              {
                this->ForwardIterator(own, offset);
              }

            BidirectionalIterator(
                const ite_type &_cp)
              {
                this->ForwardIterator(_cp);
              };

            BidirectionalIterator(
                ite_type &&_mv)
              {
                this->ForwardIterator(_mv);
              }

            ite_type &operator--()
              {
                this->_owner
                    ->prev(this->_data, this->_offset);
                return *this;
              }

            ite_type operator--(int)
              {
                ite_type tmp(*this);
                ++*this;
                return tmp;
              }

        private:
            owner *_owner = nullptr;
          };

      template<
          typename __wrapper,
          typename __owner,
          typename __tag = ::std::random_access_iterator_tag>
        class RandomAccessIterator :
            public BidirectionalIterator<
                __wrapper,
                __owner,
                __tag>
          {
        public:
            typedef ::std::iterator<
                __tag,
                __wrapper> _Base;
            typedef RandomAccessIterator<
                __wrapper,
                __owner,
                __tag> ite_type;
            typedef __wrapper wrapper;
            typedef __owner owner;
            typedef typename _Base::difference_type difference_type;
            typedef typename _Base::value_type value_type;
            typedef typename _Base::pointer pointer;
            typedef typename _Base::reference reference;
            typedef typename _Base::iterator_category iterator_category;

            RandomAccessIterator() = default;

            RandomAccessIterator(
                owner *owner,
                difference_type offset = 0)
              {
                this->BidirectionalIterator(owner, offset);
              }

            RandomAccessIterator(
                const ite_type &_cp)
              {
                this->BidirectionalIterator(_cp);
              };

            RandomAccessIterator(
                ite_type &&_mv)
              {
                this->BidirectionalIterator(_mv);
              }

            ite_type &operator+=(difference_type _n)
              {
                if (_n >= 0) while (_n--) ++*this;
                else while (_n++) --*this;
                return *this;
              }

            ite_type &operator-=(
                difference_type _n)
              {
                return *this += -_n;
              }

            difference_type operator-(
                const ite_type &it)
              {
                return it - *this;
              }

            ite_type operator-(
                difference_type _n)
              {
                ite_type tmp(*this);
                return tmp += -_n;
              }

            ite_type operator+(
                difference_type _n)
              {
                ite_type tmp(*this);
                return tmp += _n;
              }

            value_type &operator[](int n)
              {
                return *((*this) + n);
              }

            bool operator<(const ite_type &it)
              {
                return it - *this > 0;
              }

            bool operator>(const ite_type &it)
              {
                return it < *this;
              }

            bool operator<=(const ite_type &it)
              {
                return !(*this > it);
              }

            bool operator>=(const ite_type &it)
              {
                return !(*this < it);
              }

        private:
            owner *_owner = nullptr;
          };
  }
#endif //__BFP_ITERATOR_HPP
