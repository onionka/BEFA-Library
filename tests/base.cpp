//
// Created by onionka on 10/08/15.
//

#include <boost/test/unit_test.hpp>
#include <bfp.hpp>
#include "support.hpp"


BOOST_AUTO_TEST_SUITE(base_bfd)

      BOOST_AUTO_TEST_CASE(bfd_init)
        {
          auto _bfd = ::bfp::Parser::get_unique_instance();
          BOOST_CHECK(_bfd != nullptr);
        }

      BOOST_AUTO_TEST_CASE(targets)
        {
          auto _bfd = ::bfp::Parser::get_unique_instance();
          BOOST_CHECK(_bfd != nullptr);
          auto _targets = _bfd->getTargets(
              *boost::unit_test::framework::master_test_suite().argv);
          BOOST_CHECK(!_targets.empty());
          for (auto _tar : _targets)
            BOOST_MESSAGE(
                (::std::string("Target of this file: ") + _tar).c_str());
          BOOST_CHECK(
              ::std::find(_targets.begin(), _targets.end(), "elf64-x86-64") !=
              _targets.end());
          BOOST_CHECK(!_bfd->getAllTargets()
                           .empty());
        }

      BOOST_AUTO_TEST_CASE(open_file)
        {
          auto _bfd = ::bfp::Parser::get_unique_instance();
          auto _file = _bfd->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");
          BOOST_CHECK(_file != nullptr);
        }

      BOOST_AUTO_TEST_CASE(close_file)
        {
          auto _bfd = ::bfp::Parser::get_unique_instance();
          auto _file = _bfd->openedFiles
                           .back();
          _bfd->openedFiles
              .pop_back();
          BOOST_CHECK(_file != nullptr);
          delete _file;
        }

  BOOST_AUTO_TEST_SUITE_END()
