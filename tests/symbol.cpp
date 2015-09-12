//
// Created by onionka on 10/08/15.
//

#include <boost/test/unit_test.hpp>
#include <bfp.hpp>
#include "support.hpp"


#define __BOOST_MESSAGE_START(msg)\
  BOOST_MESSAGE("'_start': '" << #msg << "'='" << msg << "'")

BOOST_AUTO_TEST_SUITE(base_symbol)

      BOOST_AUTO_TEST_CASE(symbol_attr)
        {
          auto _file = ::bfp::Parser::get_unique_instance().Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");

          BOOST_MESSAGE("Checking symbol attributes");
          for (auto &_sec : _file)
            for (auto &_sym : _sec)
              {
                _sym.hasFlags();
                _sym.isLocal();
                _sym.isGlobal();
                _sym.isExported();
                _sym.isFunction();
                _sym.isDebugging();
                _sym.isWeak();
                _sym.pointsToSection();
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

      BOOST_AUTO_TEST_CASE(_start_attr)
        {
          auto _file = ::bfp::Parser::get_unique_instance().Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");
          auto _sec = ::std::find_if(_file.begin(), _file.end(),
                                     [](const ::bfp::Section &_sec)
                                       { return _sec == ".text"; });
          auto _start = ::std::find_if(_sec->begin(), _sec->end(),
                                       [](const ::bfp::Symbol &_sym)
                                         { return _sym == "_start"; });
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
          __BOOST_MESSAGE_START(_start->pointsToSection());
          __BOOST_MESSAGE_START(_start->isWarning());
          __BOOST_MESSAGE_START(_start->isWeak());
        }

      BOOST_AUTO_TEST_CASE(operators)
        {
          auto _file = ::bfp::Parser::get_unique_instance().Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");
          auto _sec = ::std::find_if(_file.begin(), _file.end(),
                                     [](const ::bfp::Section &_sec)
                                       { return _sec == ".text"; });
          auto _start = ::std::find_if(_sec->begin(), _sec->end(),
                                       [](const ::bfp::Symbol &_sym)
                                         { return _sym == "_start"; });
          BOOST_CHECK(*_start == *_start);
          BOOST_CHECK(*_start == "_start");
          BOOST_CHECK(*_start == _start->getValue());
          BOOST_CHECK(!(*_start == (asymbol *) 0));

          BOOST_CHECK(!(*_start != *_start));
          BOOST_CHECK(*_start != "start");
          BOOST_CHECK(*_start != (_start->getValue() + 1));
          BOOST_CHECK(*_start != (asymbol *) 0);
        }

  BOOST_AUTO_TEST_SUITE_END()
