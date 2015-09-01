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
/*
#include <typeinfo>
#include <execinfo.h>

ATTRIBUTE_UNUSED
static ::std::string ty_vole_back()
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
*/


namespace bfp
  {
      template<
          typename __alloc, size_t DEFAULT_BLOCK_SIZE = 4094,
          typename __distance = size_t>
        struct raw_vector
          {
            typedef __alloc allocator;
            typedef allocator *allocator_ptr;
            typedef __distance distance;

            raw_vector()
                :
                _alloc(0),
                _data(nullptr),
                _size(0)
              { }

            raw_vector(distance size)
                :
                _alloc(size_to_block(size)),
                _data((allocator *) malloc(_alloc)),
                _size(size)
              { }

            raw_vector(
                allocator *data,
                size_t size)
                :
                _alloc(size_to_block(size)),
                _data(data),
                _size(size)
              { }

            raw_vector(
                const allocator *data,
                size_t size)
                :
                _alloc(size_to_block(size)),
                _data((allocator *) data),
                _size(size)
              { }

            raw_vector &operator=(raw_vector &&_mv)
              {
                if (_data != nullptr && !copied)
                  free(_data);
                _data = _mv._data;
                _size = _mv._size;
                _alloc = _mv._alloc;
                copied = _mv.copied;
                _mv._data = nullptr;
                _mv._size = 0;
                _mv._alloc = 0;
                return *this;
              }

            raw_vector(const raw_vector &_cp)
              {
                if (_data != nullptr && !copied)
                  free(_data);
                _data = _cp._data;
                _size = _cp._size;
                _alloc = _cp._alloc;
                copied = true;
              }

            raw_vector &operator=(const raw_vector &_cp)
              {
                if (_data != nullptr && !copied)
                  free(_data);
                _data = _cp._data;
                _size = _cp._size;
                _alloc = _cp._alloc;
                copied = true;
                return *this;
              }

            allocator_ptr begin()
              {
                return _data;
              }

            allocator_ptr end()
              {
                return _data + size();
              }

            allocator_ptr get()
              {
                return _data;
              }

            operator allocator_ptr()
              {
                return get();
              }

            operator void *()
              {
                return static_cast<void *>(get());
              }

            distance size() const
              {
                return _size;
              }

            inline distance size_to_block(size_t size)
              {
                return (
                    (((size / (DEFAULT_BLOCK_SIZE / sizeof(allocator))) + 1) *
                     (DEFAULT_BLOCK_SIZE / sizeof(allocator))) *
                    sizeof(allocator));
              }

            distance allocated() const
              {
                return _alloc;
              }

            void set(allocator elem)
              {
                allocator_ptr _elem = _data;
                for (size_t i = 0;
                     i < size();
                     ++i, ++_elem)
                  *_elem = elem;
              }

            void zero()
              {
                bzero(_data, _alloc);
              }

            void set_bytes(distance size)
              {
                resize(size / sizeof(allocator));
              }

            ~raw_vector()
              {
                if (_data != nullptr && !copied)
                  free(_data);
              }

            raw_vector &resize(distance size)
              {
                _size = size;
                _resize(size);
                return *this;
              }

            void init(distance size)
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
/*
                ::std::cerr << typeid(__alloc).name() << " INIT Allocated: " <<
                _alloc << " Requested size: " << (size * sizeof(allocator)) <<
                " To be allocated: " << size_to_block(size) << " " <<
                    ty_vole_back() << ::std::endl;
*/
                _alloc = size_to_block(size);
                if ((_data = (allocator *) malloc(_alloc)) == nullptr)
                  {
                    // TODO: throw exception
                  }
              }

        private:
            void _resize(distance size)
              {
                if (_data == nullptr)
                  {
                    init(size);
                    return;
                  }
                if (_alloc >= (size * sizeof(allocator)))
                  return;
                /*::std::cerr << typeid(__alloc).name() << " Allocated: " <<
                _alloc << " Requested size: " << (size * sizeof(allocator)) <<
                " To be allocated: " << size_to_block(size) << " " <<
                    ty_vole_back() << ::std::endl;*/
                _alloc = size_to_block(size);
                if ((_data = (allocator *) realloc(_data, _alloc)) == nullptr)
                  {
                    // TODO: throw exception ...
                  }
              }

        private:
            distance _alloc = 0;
            allocator_ptr _data = nullptr;
            distance _size = 0;
            bool copied = false;
          };
  }


#endif //__BFP_RAW_VECTOR_H
