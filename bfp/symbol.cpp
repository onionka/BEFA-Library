/**
 * @file symbol.cpp
 * @author Miroslav Cibulka
 * @brief Symbol class
 */

#include <bfp.hpp>


namespace bfp
  {
      bool Symbol::operator==(
          const Symbol &_compare)
        {
          return (_compare._sym == _sym);
        }

      bool Symbol::operator!=(
          const Symbol *_compare)
        {
          return (_compare->_sym != _sym);
        }

      bool Symbol::operator==(
          const Symbol *_compare)
        {
          return (_compare->_sym == _sym);
        }

      bool Symbol::operator!=(
          const Symbol &_compare)
        {
          return (_compare._sym != _sym);
        }

      bool Symbol::operator==(
          const symvalue _compare)
        {
          return (_compare == getValue());
        }

      bool Symbol::operator!=(
          const symvalue _compare)
        {
          return (_compare != getValue());
        }

      bool Symbol::operator==(
          const asymbol *_ptr)
        {
          return _sym == _ptr;
        }

      bool Symbol::operator!=(
          const asymbol *_ptr)
        {
          return _sym != _ptr;
        }

      bool Symbol::operator==(
          const char *_compare)
        {
          return getName() == _compare;
        }

      bool Symbol::operator!=(
          const char *_compare)
        {
          return getName() != _compare;
        }

      bool operator==(
          const Symbol &_this,
          const Symbol &_compare)
        {
          return (_compare._sym == _this._sym);
        }

      bool operator!=(
          const Symbol &_this,
          const Symbol &_compare)
        {
          return (_compare._sym != _this._sym);
        }

      bool operator==(
          const Symbol &_this,
          const Symbol *_compare)
        {
          return (_compare->_sym == _this._sym);
        }

      bool operator!=(
          const Symbol &_this,
          const Symbol *_compare)
        {
          return (_compare->_sym != _this._sym);
        }

      bool operator==(
          const Symbol &_this,
          const symvalue _compare)
        {
          return (_compare == _this.getValue());
        }

      bool operator!=(
          const Symbol &_this,
          const symvalue _compare)
        {
          return (_compare != _this.getValue());
        }

      bool operator==(
          const Symbol &_this,
          const asymbol *_ptr)
        {
          return _this._sym == _ptr;
        }

      bool operator!=(
          const Symbol &_this,
          const asymbol *_ptr)
        {
          return _this._sym != _ptr;
        }

      bool operator==(
          const Symbol &_this,
          const char *_compare)
        {
          return _this.getName() == _compare;
        }

      bool operator!=(
          const Symbol &_this,
          const char *_compare)
        {
          return _this.getName() != _compare;
        }

      const ::std::string Symbol::getName() const
        {
          return ::std::string(_sym->name);
        }

      symvalue Symbol::getValue() const
        {
          return bfd_asymbol_value(_sym);
        }

      bool Symbol::hasFlags() const
        {
          return static_cast<bool>(_sym->flags == BSF_NO_FLAGS);
        }

      bool Symbol::isLocal() const
        {
          return static_cast<bool>(_sym->flags & BSF_LOCAL);
        }

      bool Symbol::isGlobal() const
        {
          return static_cast<bool>(_sym->flags & BSF_GLOBAL);
        }

      bool Symbol::isExported() const
        {
          return static_cast<bool>(_sym->flags & BSF_EXPORT);
        }

      bool Symbol::isFunction() const
        {
          return static_cast<bool>(_sym->flags & BSF_FUNCTION);
        }

      bool Symbol::isDebugging() const
        {
          return static_cast<bool>(_sym->flags & BSF_DEBUGGING);
        }

      bool Symbol::isWeak() const
        {
          return static_cast<bool>(_sym->flags & BSF_WEAK);
        }

      bool Symbol::pointsToSection() const
        {
          return static_cast<bool>(_sym->flags & BSF_SECTION_SYM);
        }

      bool Symbol::isOldCommon() const
        {
          return static_cast<bool>(_sym->flags & BSF_OLD_COMMON);
        }

      bool Symbol::isNotAtEnd() const
        {
          return static_cast<bool>(_sym->flags & BSF_NOT_AT_END);
        }

      bool Symbol::isInConstructSection() const
        {
          return static_cast<bool>(_sym->flags & BSF_CONSTRUCTOR);
        }

      bool Symbol::isWarning() const
        {
          return static_cast<bool>(_sym->flags & BSF_WARNING);
        }

      bool Symbol::isIndirect() const
        {
          return static_cast<bool>(_sym->flags & BSF_INDIRECT);
        }

      bool Symbol::hasFileName() const
        {
          return static_cast<bool>(_sym->flags & BSF_FILE);
        }

      bool Symbol::isFromDLI() const
        {
          return static_cast<bool>(_sym->flags & BSF_DYNAMIC);
        }

      bool Symbol::hasObjectData() const
        {
          return static_cast<bool>(_sym->flags & BSF_OBJECT);
        }

      Symbol::__iterator::difference_type Symbol::size()
        {
          if (_size == -1)
            _size;
          return _size;
        }

      Symbol::__iterator::difference_type Symbol::capacity()
        {
          return size();
        }

      Symbol::__iterator::difference_type Symbol::max_size()
        {
          return size();
        }

      Instruction Symbol::operator[](
          size_t n)
        {
          //return *(begin() + (int)n);
          __iterator i = begin();
          for (size_t j = 0;
               j < n;
               j++, i++);
          return ::std::move(*i);
        }


      Symbol::__iterator Symbol::begin()
        {
          auto _file = (File::__ffile *) _dis_info->stream;
          _file->pos = 0;
          int _instr_size = _dis_fun(getValue(), _dis_info);
          if (_instr_size < 0 || _instr_size >= size())
            return __iterator(this, size());
          auto _ite = __iterator(this, _instr_size);
          _ite->_address = getValue();
          _ite->_op_code = _data;
          _ite->_size = static_cast<size_t>(_instr_size);
          _ite->_s_signature = _file->buffer;
          _ite->_binary = "";
          return _ite;
        }

      Symbol::__iterator Symbol::end()
        {
          return __iterator(this, size());
        }

      void Symbol::next(
          Instruction *instr,
          __iterator::difference_type *offset)
        {
          auto _file = (File::__ffile *) _dis_info->stream;
          _file->pos = 0;
          int _instr_size = _dis_fun(getValue() + *offset, _dis_info);
          if (_instr_size < 0 || _instr_size >= size())
            {
              *offset = size();
              return;
            }
          instr->_address = getValue() + *offset;
          instr->_op_code = _data + *offset;
          instr->_size = static_cast<size_t>(_instr_size);
          instr->_s_signature = _file->buffer;
          instr->_binary = "";
          *offset += _instr_size;
        }

      Symbol::__instr_vec Symbol::getInstructions()
        {
          if (!has_no_intructions && _instructions.empty())
            {
              uint64_t next_address = this->getValue() + size();
              /*and finally decoding instructions from _sym to next symbol or end of section*/
              auto _file = (File::__ffile *) _dis_info->stream;
              if (_dis_info == nullptr)
                {
                  has_no_intructions = true;
                  return ::std::move(_instructions);
                }
              for (int _dis = 0, _instr_size = 0;
                   getValue() + _dis < next_address;
                   _dis += _instr_size)
                {
                  _file->pos = 0;
                  _instr_size = _dis_fun(getValue() + _dis, _dis_info);
                  if (_instr_size <= 0)
                    break;
                  _instructions.push_back(
                      new Instruction((_data + _dis), (size_t) _instr_size,
                                      _file->buffer, getValue() + _dis));
                }
              has_no_intructions = true;
            }
          return ::std::move(_instructions);
        }

      Symbol::Symbol(
          asymbol *symbol,
          uint8_t *data,
          disassembler_ftype dis_fun,
          disassemble_info *dis_info)
          :
          _sym(symbol),
          _data(data),
          _dis_fun(dis_fun),
          _dis_info(dis_info)
        { }

      Symbol::~Symbol()
        {
          for (auto _instr : _instructions)
            delete _instr;
        }
  }
