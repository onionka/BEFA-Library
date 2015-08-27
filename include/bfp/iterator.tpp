/**
 * @file iterator.tpp
 * @author Miroslav Cibulka
 * @brief Iterators of all kinds
 */

#ifndef __BFP_ITERATOR_HPP
#define __BFP_ITERATOR_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      template<
          typename __wrapper,
          typename __owner,
          typename __tag = ::std::forward_iterator_tag>
        class ForwardIterator :
            public ::std::iterator<
                __tag,
                __wrapper>
          {
            typedef ::std::iterator<
                __tag,
                __wrapper> _Base;
        public:
            typedef __wrapper wrapper;
            typedef __owner owner;
            typedef typename _Base::difference_type difference_type;
            typedef typename _Base::value_type value_type;
            typedef typename _Base::pointer pointer;
            typedef typename _Base::reference reference;
            typedef typename _Base::iterator_category iterator_category;

            typedef ForwardIterator<
                wrapper,
                owner,
                __tag> ite_type;

            ForwardIterator() = default;

            ForwardIterator(
                owner *owner,
                difference_type offset = 0)
                :
                _owner(owner),
                _offset(offset)
              { }

            ForwardIterator(
                const ite_type &_cp)
              {
                _data = _cp._data;
                _owner = _cp._owner;
                _offset = _cp._offset;
              }

            ForwardIterator(
                ite_type &&_mv)
              {
                ::std::swap(_data, _mv._data);
                ::std::swap(_owner, _mv._owner);
                ::std::swap(_offset, _mv._offset);
              }

            ite_type &operator=(
                const ite_type &_cp)
              {
                _data = _cp._data;
                _owner = _cp._owner;
                _offset = _cp._offset;
                return *this;
              }

            ite_type &operator=(
                ite_type &&_mv)
              {
                ::std::swap(_data, _mv._data);
                ::std::swap(_owner, _mv._owner);
                ::std::swap(_offset, _mv._offset);
                return *this;
              }

            ite_type &operator++()
              {
                _owner->next(&_data, &_offset);
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
                ::std::cout << _offset << " " << _ite._offset << ::std::endl;
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

        protected:
            value_type _data;
            owner *_owner = nullptr;
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
            typedef ::std::iterator<
                __tag,
                __wrapper> _Base;
        public:
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
                    ->prev(&this->_data, &this->_offset);
                return *this;
              }

            ite_type operator--(int)
              {
                ite_type tmp(*this);
                ++*this;
                return tmp;
              }
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
            typedef ::std::iterator<
                __tag,
                __wrapper> _Base;
        public:
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
          };
  }
#endif //__BFP_ITERATOR_HPP
