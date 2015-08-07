/**
 * @file main.c
 * @author Miroslav Cibulka
 * @brief Functional test main function
 */

#define BOOST_TEST_MODULE Functional

#include <boost/test/included/unit_test.hpp>
#include <bfp.hpp>


#undef BOOST_MESSAGE
#define BOOST_MESSAGE(msg) BOOST_TEST_MESSAGE("MESSAGE: >>> " << msg)

template<typename __vec>
  void CHECK_VECTOR(__vec &&_vec)
    {
      BOOST_CHECK(!_vec.empty());
      BOOST_CHECK(_vec.size() > 0);
      BOOST_CHECK(_vec.max_size() > 0);
      BOOST_CHECK(_vec.capacity() > 0);
      BOOST_CHECK(_vec.at(0) == _vec[0]);
      BOOST_CHECK(_vec.cbegin() != _vec.cend());
      BOOST_CHECK(_vec.crbegin() != _vec.crend());
      BOOST_CHECK(_vec.front() != _vec.back());
    }

BOOST_AUTO_TEST_SUITE(bfp_supp_functions)

      BOOST_AUTO_TEST_CASE(dereference_pointer_test)
        {
          char _c = 'a';
          char *a = &_c;
          char **b = &a;
          char ***c = &b;
          char ****d = &c;

          BOOST_CHECK(::BFP::dereference(a) == _c);
          BOOST_CHECK(::BFP::dereference(b) == _c);
          BOOST_CHECK(::BFP::dereference(c) == _c);
          BOOST_CHECK(::BFP::dereference(d) == _c);
        }

      BOOST_AUTO_TEST_CASE(find_in_string)
        {
          ::std::vector<::std::string> _strs{"text", "text2", "text3"};

          BOOST_MESSAGE(
              "finding ::std::string in ::std::vector<::std::string>");
          auto _founded = ::BFP::find(_strs.begin(), _strs.end(),
                                      ::std::string("text2"));
          BOOST_CHECK(_founded != _strs.end());
          BOOST_CHECK(*_founded == "text2");

          BOOST_MESSAGE("finding const char* in ::std::vector<::std::string>");
          _founded = ::BFP::find(_strs.begin(), _strs.end(), "text2");
          BOOST_CHECK(_founded != _strs.end());
          BOOST_CHECK(*_founded == "text2");

          BOOST_MESSAGE(
              "finding const char* in ::std::vector<::std::string> at the end of vector");
          _founded = ::BFP::find(_strs.begin(), _strs.end(), "text3");
          BOOST_CHECK(_founded != _strs.end());
          BOOST_CHECK(*_founded == "text3");
        }

      BOOST_AUTO_TEST_CASE(search_in_string)
        {
          ::std::vector<::std::string> _strs{
              "text", "text2", "text", "text3", "text"
          };

          BOOST_MESSAGE(
              "search: ::std::string in ::std::vector<::std::string>");
          auto _founded = ::BFP::search(_strs.begin(), _strs.end(),
                                        ::std::string("text"));
          BOOST_CHECK(_founded.size() == 3);
          for (auto _text : _founded)
            BOOST_CHECK(_text == "text");

          BOOST_MESSAGE("search: const char* in ::std::vector<::std::string>");
          _founded = ::BFP::search(_strs.begin(), _strs.end(), "text");
          BOOST_CHECK(_founded.size() == 3);
          for (auto _text : _founded)
            BOOST_CHECK(_text == "text");

          BOOST_MESSAGE(
              "search: const char* in ::std::vector<::std::string> at the end of vector");
          _founded = ::BFP::search(_strs.begin(), _strs.end(), "text3");
          BOOST_CHECK(_founded.size() == 1);
          for (auto _text : _founded)
            BOOST_CHECK(_text == "text3");
        }

      BOOST_AUTO_TEST_CASE(apply_on_string)
        {
          ::std::vector<::std::string> _strs{"text", "text", "text"};
          int i = 0;

          BOOST_MESSAGE(
              "apply: adding int at the end of each string in ::std::vector<::std::string>");
          ::BFP::apply(_strs.begin(), _strs.end(),
                       [&](::std::string &_str) -> void
                         {
                           _str += ::std::to_string(i++);
                         });
          i = 0;
          for (auto _str : _strs)
            {
              BOOST_CHECK(
                  _str == (::std::string("text") + ::std::to_string(i++)));
            }

          BOOST_MESSAGE("apply: checking return code of apply function");
          i = 0;
          int _ret = ::BFP::apply(_strs.begin(), _strs.end(), [](
              ::std::string &_str,
              int *_i) -> int
            {
              return (_str == "text2") ? ++(*_i) : 0;
            }, &i);
          BOOST_CHECK(_ret == i);
          BOOST_CHECK(_ret == 1);
        }

      BOOST_AUTO_TEST_CASE(filter_on_string)
        {
          ::std::vector<::std::string> _strs{
              "text", "text1", "text", "text", "text2", "text3"
          };

          BOOST_MESSAGE(
              "filter: ::std::vector<::std::string> with one lambda by reference");
          auto _founded = ::BFP::filter(_strs.begin(), _strs.end(),
                                        [](::std::string &_str) -> bool
                                          {
                                            return _str == "text";
                                          });
          BOOST_CHECK(_founded.size() == 3);
          for (auto _str : _founded)
            BOOST_CHECK(_str == "text");

          BOOST_MESSAGE(
              "filter: ::std::vector<::std::string> with two lambdas by reference");
          _founded = ::BFP::filter(_strs.begin(), _strs.end(),
                                   [](::std::string &_str) -> bool
                                     {
                                       return _str[0] == 't';
                                     }, [](::std::string &_str) -> bool
                                     {
                                       return _str.size() == 4;
                                     });
          BOOST_CHECK(_founded.size() == 3);
          for (auto _str : _founded)
            {
              BOOST_CHECK(_str == "text");
            }
        }

  BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(base_bfd)

      BOOST_AUTO_TEST_CASE(bfd_init)
        {
          auto _bfd = ::BFP::BFD::get_unique_instance();
          BOOST_CHECK(_bfd != nullptr);
        }

      BOOST_AUTO_TEST_CASE(targets)
        {
          auto _bfd = ::BFP::BFD::get_unique_instance();
          BOOST_CHECK(_bfd != nullptr);
          auto _targets = _bfd->getTargets(
              *boost::unit_test::framework::master_test_suite().argv);
          BOOST_CHECK(!_targets.empty());
          for (auto _tar : _targets)
            BOOST_MESSAGE(
                (::std::string("Target of this file: ") + _tar).c_str());
          BOOST_CHECK(::BFP::search(_targets.begin(), _targets.end(),
                                    "elf64-x86-64").size() == 1);
          BOOST_CHECK(!_bfd->getAllTargets()
                           .empty());
        }

      BOOST_AUTO_TEST_CASE(open_file)
        {
          auto _bfd = ::BFP::BFD::get_unique_instance();
          auto _file = _bfd->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");
          BOOST_CHECK(_file != nullptr);
        }

      BOOST_AUTO_TEST_CASE(close_file)
        {
          auto _bfd = ::BFP::BFD::get_unique_instance();
          auto _file = _bfd->openedFiles
                           .back();
          _bfd->openedFiles
              .pop_back();
          BOOST_CHECK(_file != nullptr);
          delete _file;
        }

  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(base_section)

      BOOST_AUTO_TEST_CASE(section_attr)
        {
          auto _file = ::BFP::BFD::get_unique_instance()->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");

          BOOST_MESSAGE("Checking section attributes");
          for (auto &_sec : *_file)
            {
              _sec.getName();
              _sec.getLineNO();
              _sec.getIndex();
              _sec.getContent();
              _sec.hasFlags();
              _sec.isAllocOnLoad();
              _sec.isLoadedWithFile();
              _sec.hasRellocInfo();
              _sec.isReadOnly();
              _sec.hasCodeOnly();
              _sec.hasDataOnly();
              _sec.isInROM();
              _sec.hasConstructorInfo();
              _sec.hasContent();
              _sec.isSuppressed();
              _sec.isCOFF();
              _sec.hasCommonSymbols();
              _sec.isDebugOnly();
              _sec.isInMemory();
              _sec.isExcluded();
              _sec.isSorted();
              _sec.linkOnce();
              _sec.linkDuplicates();
              _sec.discardDuplicates();
              _sec.linkOneDuplicate();
              _sec.linkSameSizedDuplicates();
              _sec.linkSameDuplicates();
              _sec.isCreatedByLinker();
            }
        }


#define __BOOST_MESSAGE_DOT_TEXT(msg)\
        BOOST_MESSAGE("'.text': '" << #msg << "'='" << msg << "'")

      BOOST_AUTO_TEST_CASE(dot_text_attr)
        {
          auto _file = ::BFP::BFD::get_unique_instance()->openedFiles
                                                        .back();
          auto _dot_text = ::BFP::find(_file->begin(), _file->end(), ".text");
          CHECK_VECTOR(*_dot_text);
          __BOOST_MESSAGE_DOT_TEXT((size_t) _dot_text->getContent());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->getIndex());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->getName()
                                            .c_str());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasFlags());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isAllocOnLoad());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isLoadedWithFile());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasRellocInfo());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isReadOnly());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasCodeOnly());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasDataOnly());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isInROM());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasConstructorInfo());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasContent());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isSuppressed());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isCOFF());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->hasCommonSymbols());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isDebugOnly());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isInMemory());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isExcluded());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isSorted());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->linkOnce());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->linkDuplicates());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->discardDuplicates());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->linkOneDuplicate());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->linkSameSizedDuplicates());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->linkSameDuplicates());
          __BOOST_MESSAGE_DOT_TEXT(_dot_text->isCreatedByLinker());
        }
#undef __BOOST_MESSAGE_DOT_TEXT

  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(base_symbol)

      BOOST_AUTO_TEST_CASE(symbol_attr)
        {
          auto _file = ::BFP::BFD::get_unique_instance()->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");

          BOOST_MESSAGE("Checking symbol attributes");
          for (auto &_sec : *_file)
            for (auto &_sym : _sec)
              {
                _sym.hasFlags();
                _sym.isLocal();
                _sym.isGlobal();
                _sym.isExported();
                _sym.isFunction();
                _sym.isDebugging();
                _sym.isWeak();
                _sym.isSectionSymbol();
                _sym.isOldCommon();
                _sym.isNotAtEnd();
                _sym.isInConstructSection();
                _sym.isWarning();
                _sym.isIndirect();
                _sym.hasFileName();
                _sym.isFromDLI();
                _sym.hasObjectData();
                _sym.getName();
                _sym.getValue();
              }
        }
#define __BOOST_MESSAGE_START(msg)\
  BOOST_MESSAGE("'_start': '" << #msg << "'='" << msg << "'")

      BOOST_AUTO_TEST_CASE(_start_attr)
        {
          auto _file = ::BFP::BFD::get_unique_instance()->openedFiles
                                                        .back();
          auto _sec = ::BFP::find(_file->begin(), _file->end(), ".text");
          auto _start = ::BFP::find(_sec->begin(), _sec->end(), "_start");
          __BOOST_MESSAGE_START(_start->getName());
          __BOOST_MESSAGE_START(_start->getValue());
          __BOOST_MESSAGE_START(_start->hasFileName());
          __BOOST_MESSAGE_START(_start->hasFlags());
          __BOOST_MESSAGE_START(_start->hasObjectData());
          __BOOST_MESSAGE_START(_start->isDebugging());
          __BOOST_MESSAGE_START(_start->isExported());
          __BOOST_MESSAGE_START(_start->isFromDLI());
          __BOOST_MESSAGE_START(_start->isFunction());
          __BOOST_MESSAGE_START(_start->isGlobal());
          __BOOST_MESSAGE_START(_start->isInConstructSection());
          __BOOST_MESSAGE_START(_start->isIndirect());
          __BOOST_MESSAGE_START(_start->isLocal());
          __BOOST_MESSAGE_START(_start->isNotAtEnd());
          __BOOST_MESSAGE_START(_start->isOldCommon());
          __BOOST_MESSAGE_START(_start->isSectionSymbol());
          __BOOST_MESSAGE_START(_start->isWarning());
          __BOOST_MESSAGE_START(_start->isWeak());
          BOOST_CHECK(_start->section() == ".text");
        }

#undef __BOOST_MESSAGE_START

  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(base_file)

      BOOST_AUTO_TEST_CASE(file_as_vector)
        {
          auto _file = ::BFP::BFD::get_unique_instance()->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");
          ::std::vector<::BFP::Symbol> _syms;

          BOOST_MESSAGE("Iterating symbols through this executable");
          for (auto &_sec : *_file)
            for (auto &_sym : _sec)
              _syms.push_back(_sym);
          BOOST_CHECK(!_syms.empty());

          BOOST_MESSAGE("Reverse iterating symbols through this executable");
          ::std::vector<::BFP::Symbol> _rev_syms;
          for (auto _sec_ite = _file->rbegin();
               _sec_ite != _file->rend(); _sec_ite++)
            for (auto _sym_ite = _sec_ite->rbegin();
                 _sym_ite != _sec_ite->rend(); _sym_ite++)
              _rev_syms.push_back(*_sym_ite);
          ::std::reverse(_rev_syms.begin(), _rev_syms.end());
          BOOST_CHECK(_rev_syms == _syms);

          CHECK_VECTOR(*_file);
        }

      BOOST_AUTO_TEST_CASE(file_attr)
        {
          auto _file = ::BFP::BFD::get_unique_instance()->openedFiles
                                                        .back();
          _file->get_path();
          _file->get_target();
        }

      BOOST_AUTO_TEST_CASE(finding_standards)
        {
          auto _bfd = ::BFP::BFD::get_unique_instance();
          auto _file = _bfd->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");

          BOOST_MESSAGE("Finding section '.text' in this executable");
          auto _sec_ite = ::BFP::find(_file->begin(), _file->end(), ".text");
          BOOST_CHECK(_sec_ite != _file->end());
          auto _sec = *_sec_ite;

          BOOST_MESSAGE("Finding symbol '_start' in '.text'");
          auto _sym = ::BFP::find(_sec.begin(), _sec.end(), "_start");
          BOOST_CHECK(_sym != _sec.cend());

          delete _bfd;
        }

  BOOST_AUTO_TEST_SUITE_END()
