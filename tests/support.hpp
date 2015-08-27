//
// Created by onionka on 10/08/15.
//

#ifndef __TESTS_SUPPORT_HPP
# define __TESTS_SUPPORT_HPP

#include <boost/test/unit_test.hpp>


#undef BOOST_MESSAGE
# define BOOST_MESSAGE(msg) BOOST_TEST_MESSAGE("MESSAGE: >>> " << msg)

template<typename __vec>
  void CHECK_VECTOR(__vec &&_vec)
    {
      /*BOOST_CHECK(!_vec.empty());*/
      BOOST_CHECK(_vec.size() > 0);
      BOOST_CHECK(_vec.max_size() > 0);
      BOOST_CHECK(_vec.capacity() > 0);/*
      BOOST_CHECK(_vec.at(0) == _vec[0]);
      BOOST_CHECK(_vec.cbegin() != _vec.cend());
      BOOST_CHECK(_vec.crbegin() != _vec.crend());
      BOOST_CHECK(_vec.front() != _vec.back());*/
    }


#endif //__TESTS_SUPPORT_HPP
