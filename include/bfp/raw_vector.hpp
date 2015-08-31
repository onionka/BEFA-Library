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
 *          { fclose(f); exit(3); }// ERROR
 *
 *        /// process data
 *
 *        fclose(f);
 *      @endcode
 */

#ifndef __BFP_RAW_VECTOR_H
#define __BFP_RAW_VECTOR_H


#include <string.h>
#include <iostream>


namespace bfp
  {
#define DEFAULT_BLOCK_SIZE 2048

#define SIZE_TO_BLOCKS(size)        \
      (size * sizeof(allocator))

      template<
          typename __alloc>
        struct raw_vector
          {
            typedef __alloc allocator;
            typedef allocator *allocator_ptr;
            typedef size_t distance;

            raw_vector()
                :
                _alloc(0),
                _data(nullptr),
                _size(0)
              { }

            raw_vector(distance size)
                :
                _alloc(SIZE_TO_BLOCKS(size)),
                _data((allocator *) malloc(_alloc)),
                _size(size)
              { }

            raw_vector(
                allocator *data,
                size_t size)
                :
                _alloc(SIZE_TO_BLOCKS(size)),
                _data(data),
                _size(size)
              { }

            raw_vector(raw_vector &&_mv)
                :
                _alloc(_mv._alloc),
                _data(_mv._data),
                _size(_mv._size)
              {
                _mv._data = nullptr;
                _mv._size = 0;
                _mv._alloc = 0;
              }

            raw_vector &operator=(raw_vector &&_mv)
              {
                _data = _mv._data;
                _size = _mv._size;
                _alloc = _mv._alloc;
                _mv._data = nullptr;
                _mv._size = 0;
                _mv._alloc = 0;
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
                if (_data != nullptr)
                  free(_data);
              }

            raw_vector &resize(distance size)
              {
                if (size == 0)
                  {
                    _size = 0;
                    _alloc = 0;
                    free(_data);
                  }
                else
                  _resize(size);
                return *this;
              }

            void init(distance size)
              {
                if (_data != nullptr)
                  {
                    _size = 0;
                    _alloc = 0;
                    free(_data);
                  }
                _alloc = SIZE_TO_BLOCKS(size);
                _size = size;
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
                if (_alloc > size)
                    return;
                _alloc = SIZE_TO_BLOCKS(size);
                _size = size;
                if ((_data = (allocator *) realloc(_data, _alloc)) == nullptr)
                  {
                    // TODO: throw exception ...
                  }
              }

            raw_vector(const raw_vector &) = delete;

            raw_vector &operator=(const raw_vector &) = delete;

            raw_vector operator++(int) = delete;

        private:
            distance _alloc;
            allocator_ptr _data;
            distance _size;
          };
  }


#endif //__BFP_RAW_VECTOR_H
