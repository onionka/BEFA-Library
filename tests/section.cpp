//
// Created by onionka on 10/08/15.
//

#include <boost/test/unit_test.hpp>
#include <bfp.hpp>
#include "support.hpp"


#define __BOOST_MESSAGE_DOT_TEXT(msg)\
        BOOST_MESSAGE("'.text': '" << #msg << "'='" << msg << "'")

BOOST_AUTO_TEST_SUITE(base_section)

      BOOST_AUTO_TEST_CASE(section_attr)
        {
          auto _file = ::bfp::Parser::get_unique_instance()->Open(
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

      BOOST_AUTO_TEST_CASE(dot_text_attr)
        {
          auto _file = ::bfp::Parser::get_unique_instance()->openedFiles
                                                           .back();
          auto _dot_text = ::std::find_if(_file->begin(), _file->end(),
                                          [](const ::bfp::Section &_sec)
                                            { return _sec == ".text"; });
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

      BOOST_AUTO_TEST_CASE(as_vector)
        {
          auto _file = ::bfp::Parser::get_unique_instance()->openedFiles
                                                           .back();
          auto _dot_text = ::std::find_if(_file->begin(), _file->end(),
                                          [](const ::bfp::Section &_sec)
                                            { return _sec == ".text"; });
          CHECK_VECTOR(*_dot_text);
        }

      BOOST_AUTO_TEST_CASE(operators)
        {
          auto _file = ::bfp::Parser::get_unique_instance()->openedFiles
                                                           .back();
          auto _dot_text = ::std::find_if(_file->begin(), _file->end(),
                                          [](const ::bfp::Section &_sec)
                                            { return _sec == ".text"; });
          BOOST_CHECK(*_dot_text == *_dot_text);
          BOOST_CHECK(*_dot_text == ".text");
          BOOST_CHECK(!(*_dot_text == (asection *) 0));

          BOOST_CHECK(!(*_dot_text != *_dot_text));
          BOOST_CHECK(*_dot_text != "text");
          BOOST_CHECK(*_dot_text != (asection *) 0);
        }

  BOOST_AUTO_TEST_SUITE_END()
