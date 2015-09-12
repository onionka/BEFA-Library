/**
 * @file parser.cpp
 * @author Miroslav Cibulka
 * @brief Source where Binary File Descriptor is implemented
 */

#include "../include/bfp.hpp"


namespace bfp
  {
      Parser Parser::instance;

      Parser &Parser::get_unique_instance()
        {
          return instance;
        }

      File Parser::Open(
          const ::std::string _file_name,
          ::std::string _target)
        {
          bfd *fd;
          if (_target == "")
            {
              auto targets = getTargets(_file_name);
              if (targets.size() > 1)
                _target = targets.front();
              else
                throw ::std::runtime_error("File not exists!");
            }
          else if (!checkTarget(_file_name, _target))
            {
              throw ::std::runtime_error(
                  ::std::string("Bad target for '") + _file_name + "' (" +
                  _target + ")\n");
            }
          if ((fd = bfd_openr(_file_name.c_str(), _target.c_str())) == NULL)
            throw ::std::runtime_error(
                ::std::string("Cannot open '") + _file_name + "' with target " +
                _target + "\n");
          if (!bfd_check_format(fd, bfd_object))
            {
              bfd_close(fd);
              throw ::std::runtime_error(
                  ::std::string("Cannot open '") + _file_name +
                  "' as object binary file\n");
            }
          return File(fd, _file_name.c_str(), _target.c_str());
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
        { }

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
          ::std::string _target)
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

      Parser::Parser(const Parser &_cp)
        {
          _targets = _cp._targets;
        }

      Parser::Parser(Parser &&_mv)
        {
          _targets = ::std::move(_mv._targets);
        }

      Parser &Parser::operator=(const Parser &_cp)
        {
          _targets = _cp._targets;
          return *this;
        }

      Parser &Parser::operator=(Parser &&_mv)
        {
          _targets = ::std::move(_mv._targets);
          return *this;
        }
  }
