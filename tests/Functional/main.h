/**
 * @file main.h
 * @author Miroslav Cibulka
 * @brief Functional test defines
 */

#ifndef BINARYFILEPARSER_MAIN_H
#define BINARYFILEPARSER_MAIN_H

#define BOOST_TEST_MODULE Functional

#include <boost/test/included/unit_test.hpp>
#include <bfp.hpp>
#include <iostream>


BOOST_AUTO_TEST_SUITE(bfp_supp_functions)

      BOOST_AUTO_TEST_CASE(find_in_string)
        {
          ::std::vector<::std::string> _strs{"text", "text2", "text3"};

          BOOST_MESSAGE(
              "finding ::std::string in ::std::vector<::std::string>");
          auto _founded = ::BFP::find(_strs.begin(), _strs.end(),
                                      ::std::string("text2"));
          BOOST_CHECK(_founded != _strs.end());
          BOOST_CHECK(*_founded == "text2");

          BOOST_MESSAGE(
              "finding const char* in ::std::vector<::std::string>");
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
              "searching ::std::string in ::std::vector<::std::string>");
          auto _founded = ::BFP::search(_strs.begin(), _strs.end(),
                                        ::std::string("text"));
          BOOST_CHECK(_founded.size() == 3);
          for (auto _text : _founded)
            BOOST_CHECK(_text == "text");

          BOOST_MESSAGE(
              "searching const char* in ::std::vector<::std::string>");
          _founded = ::BFP::search(_strs.begin(), _strs.end(), "text");
          BOOST_CHECK(_founded.size() == 3);
          for (auto _text : _founded)
            BOOST_CHECK(_text == "text");

          BOOST_MESSAGE(
              "searching const char* in ::std::vector<::std::string> at the end of vector");
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

          BOOST_MESSAGE(
              "apply: checking return code of apply function");
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
              "filtering ::std::vector<::std::string> with one lambda by reference");
          auto _founded = ::BFP::filter(_strs.begin(), _strs.end(),
                                        [](::std::string &_str) -> bool
                                          {
                                            return _str == "text";
                                          });
          BOOST_CHECK(_founded.size() == 3);
          for (auto _str : _founded)
            BOOST_CHECK(_str == "text");

          BOOST_MESSAGE(
              "filtering ::std::vector<::std::string> with two lambdas by reference");
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

      BOOST_AUTO_TEST_CASE(test_targets)
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
        }

      BOOST_AUTO_TEST_CASE(bfd_free)
        {
          auto _bfd = ::BFP::BFD::get_unique_instance();
          delete _bfd;
          _bfd = nullptr;
        }

  BOOST_AUTO_TEST_SUITE_END()

#endif //BINARYFILEPARSER_MAIN_H
