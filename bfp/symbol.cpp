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

      Symbol::__section Symbol::section()
        {
          return _section;
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

      Symbol::__iterator Symbol::begin()
        {
          __iterator _ite(this);
          return _ite;
        }

      Symbol::__iterator Symbol::end()
        {
          __iterator _ite(this, section()->getLastAddress());
          return _ite;
        }

      size_t Symbol::size()
        {
          if (_size == -1)
            _size = (int64_t) (section()->getNearestAddress(getValue()) -
                               getValue());
          return (size_t) _size;
        }

      size_t Symbol::capacity()
        {
          return size();
        }

      size_t Symbol::max_size()
        {
          return size();
        }

      Instruction &Symbol::operator[](
          size_t n)
        {
          return *(__iterator(this, n));
        }

      void Symbol::next(Instruction *_data)
        {
          auto _dis_asm = _parent->getDisassembler();
          auto _dis_asm_info = section()->getDisassembleInfo();
          uint64_t nearest_address = section()->getNearestAddress(
              _data->_address);
          _parent->_FFILE.pos = 0;
          int _instr_size = _dis_asm(nearest_address, _dis_asm_info);
          _data->_address = nearest_address;
          if (_instr_size < 0)
            {
              return;
            }
          _data->realloc((size_t)_instr_size);
          memcpy(_data->_op_code, section()->getContent() - nearest_address -
                                  section()->getAddress(),
                 (size_t) _instr_size);
          _data->_s_signature = _parent->_FFILE.buffer;
          _data->_binary = "";
        }

      void Symbol::prev(Instruction *_data)
        {
          auto _dis_asm = _parent->getDisassembler();
          auto _dis_asm_info = section()->getDisassembleInfo();
          uint64_t nearest_address = section()->getNearestPrevAddress(
              _data->_address);
          _parent->_FFILE.pos = 0;
          int _instr_size = _dis_asm(nearest_address, _dis_asm_info);
          _data->_address = nearest_address;
          if (_instr_size < 0)
            {
              return;
            }
          _data->realloc((size_t)_instr_size);
          memcpy(_data->_op_code, section()->getContent() - nearest_address -
                                  section()->getAddress(),
                 (size_t) _instr_size);
          _data->_s_signature = _parent->_FFILE.buffer;
          _data->_binary = "";
        }

      Symbol::__instr_vec &&Symbol::getInstructions()
        {
          if (!has_no_intructions && _instructions.empty())
            {
              /*and finally decoding instructions from _sym to next symbol or end of section*/
              auto _dis_asm = _parent->getDisassembler();
              auto _dis_asm_info = section()->getDisassembleInfo();
              if (_dis_asm_info == nullptr)
                {
                  has_no_intructions = true;
                  return ::std::move(_instructions);
                }
              for (int _dis = 0, _instr_size = 0;
                   getValue() + _dis < section()->getNearestAddress(this);
                   _dis += _instr_size)
                {
                  _parent->_FFILE.pos = 0;
                  _instr_size = _dis_asm(getValue() + _dis, _dis_asm_info);
                  if (_instr_size <= 0)
                    break;
                  _instructions.push_back(new Instruction(
                      (section()->getContent() + getValue() -
                       section()->getAddress() + _dis), (size_t) _instr_size,
                      _parent->_FFILE.buffer,
                      getValue() + _dis));
                }

              has_no_intructions = true;
            }
          return ::std::move(_instructions);
        }

      Symbol::Symbol(
          asymbol *symbol,
          File *parent)
          :
          _sym(symbol),
          _parent(parent)
        { }

      Symbol::~Symbol()
        {
          for (auto _instr : _instructions)
            delete _instr;
        }
  }
