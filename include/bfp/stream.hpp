//
// Created by onionka on 01/09/15.
//

#ifndef BINARYFILEPARSER_STREAM_HPP
#define BINARYFILEPARSER_STREAM_HPP

namespace bfp
  {
      struct ffile
        {
          typedef sized_raw_vector<
              char,
              128> buffer_t;

          ffile();

          void init();

          buffer_t _buffer;
          size_t pos;
        };

      /**
       * @brief Our custom fake printf that stores output from disassembler
       * @param f our fake file
       * @param format string
       * @param ... formating info
       * @return number of written bytes to f (FFILE)
       */
      int ffprintf(
          struct ffile *f,
          const char *format,
          ...);
  }

#endif //BINARYFILEPARSER_STREAM_HPP
