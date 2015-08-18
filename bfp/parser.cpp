/**
 * @file parser.cpp
 * @author Miroslav Cibulka
 * @brief Source where Binary File Descriptor is implemented
 */

#include <bfp.hpp>


namespace bfp
  {
      Parser *Parser::get_unique_instance()
        {
          static Parser *instance = new Parser();
          return instance;
        }

      File *Parser::Open(
          const ::std::string _file_name,
          const ::std::string _target)
        {
          bfd *fd;
          ::std::string _t = _target;
          if (_target == "")
            _t = getTargets(_file_name)[0];
          else if (!checkTarget(_file_name, _target))
            {
              bfd_set_error(bfd_error::bfd_error_invalid_target);
              return nullptr;
            }
          if ((fd = bfd_openr(_file_name.c_str(), _t.c_str())) == NULL)
            return nullptr;
          if (!bfd_check_format(fd, bfd_object))
            {
              bfd_close(fd);
              return nullptr;
            }
          openedFiles.push_back(
              new File(fd, _file_name.c_str(), _target.c_str()));
          return openedFiles.back();
        }

      Parser::Parser()
        {
          bfd_init();
          auto _bfd_targets = bfd_target_list();
          for (auto _target = _bfd_targets;
               *_target != NULL;
               ++_target)
            _targets.push_back(::std::string(*_target));
          free(_bfd_targets);
        }

      Parser::~Parser()
        {
          for (auto f : openedFiles)
            delete f;
          openedFiles.clear();
        }

      ::std::vector<::std::string> Parser::getTargets(const ::std::string _file_name)
        {
          ::std::vector<::std::string> _ret;
          for (auto target : _targets)
            if (checkTarget(_file_name, target))
              _ret.push_back(target);
          return _ret;
        }

      bool Parser::checkTarget(
          const ::std::string _file_name,
          const ::std::string _target)
        {
          bfd *fd;
          if ((fd = bfd_openr(_file_name.c_str(), _target.c_str())) != NULL)
            {
              bfd_close(fd);
              return true;
            }
          return false;
        }

      ::std::vector<::std::string> Parser::getAllTargets() const
        {
          return _targets;
        }
  }
