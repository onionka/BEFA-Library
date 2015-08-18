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

      Symbol::__instr_vec &Symbol::getInstructions()
        {
          if (!has_no_intructions && _instructions.empty())
            {
              /*and finally decoding instructions from _sym to next symbol or end of section*/
              auto _dis_asm = _parent->getDisassembler();
              auto _dis_asm_info = section()->getDisassembleInfo();
              if (_dis_asm_info == nullptr)
                {
                  has_no_intructions = true;
                  return _instructions;
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
                      getValue() - section()->getAddress() + _dis));
                }

              has_no_intructions = true;
            }
          return _instructions;
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
