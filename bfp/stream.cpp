/**
 *
 */

#include <bfp/raw_vector.hpp>
#include <bfp/stream.hpp>
#include <stdarg.h>


namespace bfp
  {
      int ffprintf(
          struct ffile *f,
          const char *format,
          ...)
        {
          int n = 0;
          va_list args;
          while (true)
            {
              size_t space = f->_buffer
                              .size() - f->pos;
              va_start (args, format);
              n = vsnprintf(f->_buffer + f->pos, space, format, args);
              va_end (args);
              if (space > (size_t) n)
                break;
              f->_buffer
               .resize((size_t) f->pos + n);
            }
          f->pos += n;
          return n;
        }

      ffile::ffile()
          :
          _buffer(128),
          pos(0)
        { }

      void ffile::init()
        {
          pos = 0;
          _buffer.zero();
        }
  }