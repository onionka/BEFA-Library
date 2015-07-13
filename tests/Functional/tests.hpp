/**
 * @file main.h
 * @author Miroslav Cibulka
 * @brief HERE goes all tests
 */

#ifndef BINARYFILEPARSER_TESTS_HPP
#define BINARYFILEPARSER_TESTS_HPP

#include "main.h"


DEFINE_TEST_SUIT(functional_suite) {
    DEFINE_TEST(Opening_libsample_so, {
      auto __bfd = ::BFP::BFD::get_unique_instance();
      try
        {
          auto file = __bfd->Open(*argv, "elf64-x86-64");
          return file == nullptr ? 1 : 0;
        } catch (::BFP::base_exception &ex)
        {
          ::std::cerr << ex.what();
          return 1;
        }
    }), DEFINE_TEST(Opening_libsample_so_FAIL, {
      auto __bfd = ::BFP::BFD::get_unique_instance();
      try
        {
          auto file = __bfd->Open("Not Existing Binary", "elf64-x86-64");
          return file == nullptr ? 0 : 1;
        } catch (::BFP::base_exception &ex)
        {
          ::std::cerr << ex.what();
          return 1;
        }
    }), DEFINE_TEST(Finding_DOT_text_In_Section, {
      ::BFP::File *_file = ::BFP::BFD::get_unique_instance()->openedFiles
                                                            .back();
      auto _section = ::BFP::search(_file->begin_section(),
                                    _file->end_section(), ".text");
      assert(_section.size() == 1);
      for (auto _sec : _section)
          if (_sec->getName() != ".text")
            return 1;
      assert(!_section[0]->symbols().empty());
      return 0;
    }), DEFINE_TEST(Finding_Start_In_Symbols, {
      auto _file = ::BFP::BFD::get_unique_instance()->openedFiles
                                                    .back();
      auto _symbol = ::BFP::search(_file->begin_symbol(), _file->end_symbol(),
                                   "_start");
      assert(_symbol.size() == 1);
      for (auto _sym : _symbol)
          if (_sym->getName() != "_start")
            return 1;
      assert(_symbol[0]->sections().size() == 1);
      return 0;
    }), DEFINE_TEST(Closing_One_File_BFD, {
      auto __bfd = ::BFP::BFD::get_unique_instance();
      try
        {
          delete __bfd->openedFiles
                      .back();
          __bfd->openedFiles
               .pop_back();
          return 0;
        } catch (::BFP::base_exception &ex)
        {
          ::std::cerr << ex.what();
          return 1;
        }
    }), DEFINE_TEST(Dealocate_bfp_bfd, {
      auto __bfd = ::BFP::BFD::get_unique_instance();
      try
        {
          delete __bfd;
          return 0;
        } catch (::BFP::base_exception &ex)
        {
          ::std::cerr << ex.what();
          return 1;
        }
    }),
};

#endif //BINARYFILEPARSER_TESTS_HPP
