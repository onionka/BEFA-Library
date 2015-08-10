//
// Created by onionka on 10/08/15.
//

#include <boost/test/unit_test.hpp>
#include <bfp.hpp>
#include "support.hpp"


BOOST_AUTO_TEST_SUITE(base_file)

      BOOST_AUTO_TEST_CASE(file_as_vector)
        {
          auto _file = ::bfp::Parser::get_unique_instance()->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");
          ::std::vector<::bfp::Symbol> _syms;

          BOOST_MESSAGE("Iterating symbols through this executable");
          for (auto &_sec : *_file)
            for (auto &_sym : _sec)
              _syms.push_back(_sym);
          BOOST_CHECK(!_syms.empty());

          BOOST_MESSAGE("Reverse iterating symbols through this executable");
          ::std::vector<::bfp::Symbol> _rev_syms;
          for (auto _sec_ite = _file->rbegin();
               _sec_ite != _file->rend(); _sec_ite++)
            for (auto _sym_ite = _sec_ite->rbegin();
                 _sym_ite != _sec_ite->rend(); _sym_ite++)
              _rev_syms.push_back(*_sym_ite);
          ::std::reverse(_rev_syms.begin(), _rev_syms.end());
          BOOST_CHECK(_rev_syms.size() == _syms.size());
          BOOST_CHECK(_rev_syms == _syms);

          CHECK_VECTOR(*_file);
        }

      BOOST_AUTO_TEST_CASE(file_attr)
        {
          auto _file = ::bfp::Parser::get_unique_instance()->openedFiles
                                                        .back();
          _file->get_path();
          _file->get_target();
        }

      BOOST_AUTO_TEST_CASE(finding_standards)
        {
          auto _bfd = ::bfp::Parser::get_unique_instance();
          auto _file = _bfd->Open(
              *boost::unit_test::framework::master_test_suite().argv,
              "elf64-x86-64");

          BOOST_MESSAGE("Finding section '.text' in this executable");
          auto _sec_ite = ::bfp::find(_file->begin(), _file->end(), ".text");
          BOOST_CHECK(_sec_ite != _file->end());
          auto _sec = *_sec_ite;

          BOOST_MESSAGE("Finding symbol '_start' in '.text'");
          auto _sym = ::bfp::find(_sec.begin(), _sec.end(), "_start");
          BOOST_CHECK(_sym != _sec.cend());

          delete _bfd;
        }

  BOOST_AUTO_TEST_SUITE_END()
