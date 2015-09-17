//
// Created by onionka on 16/09/15.
//

#ifndef BINARYFILEPARSER_LINE_INFO_HPP
#define BINARYFILEPARSER_LINE_INFO_HPP

namespace bfp
  {
      class LineInfo
        {
      public:
          const char *get_filename() const noexcept
            {
              return _filename;
            }

          const char *get_functionname() const noexcept
            {
              return _functionname;
            }

          unsigned int get_linenumber() const noexcept
            {
              return _linenumber;
            }

          unsigned int get_discriminator() const noexcept
            {
              return _discriminator;
            }

          LineInfo() = default;

          LineInfo(
              const char *filename,
              const char *functionname,
              unsigned linenumber,
              unsigned discriminator) noexcept
              :
              _filename(filename),
              _functionname(functionname),
              _linenumber(linenumber),
              _discriminator(discriminator)
            { }

      private:
          const char *_filename = nullptr;
          const char *_functionname = nullptr;
          unsigned _linenumber = 0;
          unsigned _discriminator = 0;
        };
  }
#endif //BINARYFILEPARSER_LINE_INFO_HPP
