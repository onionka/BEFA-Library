/**
 * @file raw_vector.hpp
 * @autor Miroslav Cibulka
 * @brief This is implementation of raw vector
 *        it is missing in STL - there is no
 *        uninitialized-data container that I know about
 * @details
 *    Easy to use, self-cleaning, high-speed vector
 *    that contains uninitialized, allocated data.
 *
 *    Example of use:
 *      @code
 *        /// opening file as binary
 *        FILE *f = fopen("data.raw", "rb");
 *
 *        /// Checking size of file
 *        fseek (f , 0 , SEEK_END);
 *        long lSize = ftell (f);
 *        rewind (f);
 *
 *        /// We will set size of data to lSize bytes
 *        raw_vector<uint8_t> data((size_t)lSize);
 *
 *        if (fread(data, 1, lSize, f) < lSize)
 *          { fclose(f); exit(3); }/// ERROR
 *
 *        for (auto _data: data)
 *          {
 *            /// process data
 *          }
 *
 *        fclose(f);
 *      @endcode
 */

#ifndef __BFP_RAW_VECTOR_H
#define __BFP_RAW_VECTOR_H


#include <string.h>
#include <iostream>


#ifdef M_C_SEMANTIC_LOG
#include <typeinfo>
#include <execinfo.h>


ATTRIBUTE_UNUSED
static inline ::std::string backtrace()
  {
    void *buffer[200];
    int n;
    char **strings;
    ::std::string msg;
    n = backtrace(buffer, 200);
    strings = backtrace_symbols(buffer, n);
    if (strings == NULL)
      {
        msg += "Error on backtracking\n";
        return msg;
      }
    else
      {
        msg += ::std::string("\nFunctions backtrace:\n");
      }
    for (int j = n - 1;
         j >= 0;
         j--)
      {
        msg += ::std::string("\t") + std::to_string(n - j) + ": " +
               ::std::string(strings[j]) + "\n";
      }
    free(strings);
    return msg;
  }
#endif //M_C_SEMANTIC_LOG

namespace bfp
  {
      template<
          typename __alloc,
          typename __distance = size_t>
        class raw_vector
          {
        public:
            typedef __alloc value_type;
            typedef __alloc *pointer;
            typedef __alloc *const_pointer;
            typedef __alloc &reference;
            typedef __alloc &const_reference;
            typedef raw_vector<
                __alloc,
                __distance> vector;
            typedef __gnu_cxx::__normal_iterator<
                pointer,
                vector> iterator;
            typedef __gnu_cxx::__normal_iterator<
                const_pointer,
                vector> const_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
            typedef std::reverse_iterator<iterator> reverse_iterator;
            typedef size_t size_type;
            typedef ::std::ptrdiff_t difference_type;
            typedef __alloc allocator_type;

        public:
            virtual iterator begin() = 0;

            virtual iterator end() = 0;

            virtual pointer get(difference_type n = 0) const = 0;

            virtual operator pointer() = 0;

            virtual operator void *() = 0;

            virtual size_type size() const = 0;

            virtual void push_back(value_type elem) = 0;

            virtual value_type back() = 0;

            virtual value_type front() = 0;

            virtual inline size_type size_to_block(size_type size) = 0;

            virtual void set(value_type elem) = 0;

            virtual void zero() = 0;

            virtual void set_bytes(difference_type size) = 0;

            virtual vector &resize(size_type size) = 0;

            virtual void init(size_type size) = 0;

            virtual size_type alloc() const = 0;

        protected:
            virtual void set_data(pointer arr) noexcept = 0;

            virtual void set_size(size_type size) noexcept = 0;

            virtual void set_alloc(size_type alloc) noexcept = 0;

            virtual void nullify() noexcept = 0;

        public:
            vector &clone(const vector &_cp) noexcept
              {
                if (_cp.get() == nullptr)
                  return *this;
                resize(_cp.size());
                memcpy(get(), _cp.get(), size());
                return *this;
              }

            vector &move(vector &_mv) noexcept
              {
                if (_mv.get() == nullptr)
                  return *this;
                if (get() != nullptr)
                  free(get());
                set_data(_mv.get());
                set_size(_mv.size());
                set_alloc(_mv.alloc());
                _mv.nullify();
                return *this;
              }

            raw_vector &operator=(const raw_vector &_cp)
              {
#ifdef M_C_SEMANTIC_LOG
                ::std::cerr << "Copy semantics were called!" << ::std::endl <<
                backtrace() << ::std::endl;
#endif
                return clone(_cp);
              }

            raw_vector &operator=(raw_vector &&_mv) noexcept
              {
#ifdef M_C_SEMANTIC_LOG

                ::std::cerr << "Move semantics were called!" << ::std::endl <<
                backtrace() << ::std::endl;
#endif
                return move(_mv);
              }

            virtual ~raw_vector()
              { }
          };

      template<
          typename __alloc, size_t DEFAULT_BLOCK_SIZE = 4094,
          typename __distance = size_t>
        class sized_raw_vector :
            public raw_vector<
                __alloc,
                __distance>
          {
        public:
            typedef raw_vector<
                __alloc,
                __distance> _Base;
            typedef typename _Base::value_type value_type;
            typedef typename _Base::pointer pointer;
            typedef typename _Base::const_pointer const_pointer;
            typedef typename _Base::reference reference;
            typedef typename _Base::const_reference const_reference;
            typedef typename _Base::vector vector;
            typedef typename _Base::iterator iterator;
            typedef typename _Base::const_iterator const_iterator;
            typedef typename _Base::const_reverse_iterator const_reverse_iterator;
            typedef typename _Base::reverse_iterator reverse_iterator;
            typedef typename _Base::size_type size_type;
            typedef typename _Base::difference_type difference_type;
            typedef typename _Base::allocator_type allocator_type;

            sized_raw_vector()
                :
                _alloc(0),
                _data(nullptr),
                _size(0)
              { }

            sized_raw_vector(size_type size)
                :
                _alloc(size_to_block(size)),
                _data((allocator_type *) malloc(_alloc)),
                _size(size)
              { }

            sized_raw_vector(
                allocator_type *data,
                size_type size)
                :
                _alloc(size_to_block(size)),
                _data(data),
                _size(size)
              { }

            sized_raw_vector(
                const allocator_type *data,
                size_type size)
                :
                _alloc(size_to_block(size)),
                _data((allocator_type *) data),
                _size(size)
              { }

            sized_raw_vector(const sized_raw_vector &_cp)
              {
#ifdef M_C_SEMANTIC_LOG
                ::std::cerr << "Copy cons were called!" << ::std::endl <<
                backtrace() << ::std::endl;
#endif
                clone(_cp);
              }

            sized_raw_vector(sized_raw_vector &&_mv) noexcept
              {
#ifdef M_C_SEMANTIC_LOG
                ::std::cerr << "Move cons were called!" << ::std::endl <<
                backtrace() << ::std::endl;
#endif
                move(_mv);
              }

            sized_raw_vector &operator=(const sized_raw_vector &_cp)
              {
#ifdef M_C_SEMANTIC_LOG
                ::std::cerr << "Copy semantics were called!" << ::std::endl <<
                backtrace() << ::std::endl;
#endif
                return clone(_cp);
              }

            sized_raw_vector &operator=(sized_raw_vector &&_mv) noexcept
              {
#ifdef M_C_SEMANTIC_LOG
                ::std::cerr << "Move semantics were called!" << ::std::endl <<
                backtrace() << ::std::endl;
#endif
                return move(_mv);
              }

            sized_raw_vector &clone(const sized_raw_vector &_cp) noexcept
              {
                if (_cp.get() == nullptr)
                  return *this;
                resize(_cp.size());
                memcpy(get(), _cp.get(), size());
                return *this;
              }

            sized_raw_vector &move(sized_raw_vector &_mv) noexcept
              {
                if (_mv.get() == nullptr)
                  return *this;
                if (get() != nullptr)
                  free(get());
                set_data(_mv.get());
                set_size(_mv.size());
                set_alloc(_mv.alloc());
                _mv.nullify();
                return *this;
              }

            virtual iterator begin()
              { return iterator(_data); }

            virtual iterator end()
              { return iterator(_data + size()); }

            virtual pointer get(difference_type n = 0) const
              { return _data + n; }

            virtual operator pointer()
              { return get(); }

            virtual operator void *()
              { return static_cast<void *>(get()); }

            virtual size_type size() const
              { return _size; }

            virtual void push_back(value_type elem)
              {
                resize(size() + 1);
                _data[size() - 1] = elem;
              }

            virtual value_type back()
              { return _data[size() - 1]; }

            virtual value_type front()
              { return *_data; }

            virtual inline size_type size_to_block(size_type size)
              {
                return (
                    (((size / (DEFAULT_BLOCK_SIZE / sizeof(value_type))) + 1) *
                     (DEFAULT_BLOCK_SIZE / sizeof(value_type))) *
                    sizeof(value_type));
              }

            virtual void set(value_type elem)
              {
                pointer _elem = get();
                for (size_t i = 0;
                     i < size();
                     ++i, ++_elem)
                  *_elem = elem;
              }

            virtual void zero()
              { bzero(get(), alloc()); }

            virtual void set_bytes(difference_type size)
              { resize(size / sizeof(value_type)); }

            virtual ~sized_raw_vector()
              {
                if (get() != nullptr)
                  free(get());
              }

            virtual vector &resize(size_type size)
              {
                _size = size;
                _resize(size);
                return *this;
              }

            virtual void init(size_type size)
              {
                _size = size;
                if (_data != nullptr)
                  {
                    _size = 0;
                    _alloc = 0;
                    free(_data);
                  }
                if (size == 0)
                  return;
                _alloc = size_to_block(size);
                if ((_data = (value_type *) malloc(_alloc)) == nullptr)
                  {
                    // TODO: throw exception
                  }
              }

            virtual size_type alloc() const
              { return _alloc; }

        protected:
            virtual void set_data(pointer arr) noexcept
              { _data = arr; }

            virtual void set_size(size_type size) noexcept
              { _size = size; }

            virtual void set_alloc(size_type alloc) noexcept
              { _alloc = alloc; }

            virtual void nullify() noexcept
              {
                _data = nullptr;
                _size = _alloc = 0;
              }

        private:
            void _resize(size_type size)
              {
                if (_data == nullptr)
                  {
                    init(size);
                    return;
                  }
                if (_alloc >= (size * sizeof(value_type)))
                  return;
                _alloc = size_to_block(size);
                if ((_data = (value_type *) realloc(_data, _alloc)) == nullptr)
                  {
                    // TODO: throw exception ...
                  }
              }

        private:
            size_type _alloc = 0;
            pointer _data = nullptr;
            size_type _size = 0;
          };
  }


#endif //__BFP_RAW_VECTOR_H
