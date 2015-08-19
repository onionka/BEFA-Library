//
// Created by onionka on 15/08/15.
//

#include <bfp.hpp>
#include <sstream>


namespace bfp
  {
      Instruction::Instruction(
          uint8_t *op_code,
          size_t count,
          const char *signature,
          Instruction::__address_t address)
          :
          _s_signature(signature),
          _address(address),
          _size(count),
          _name("")
        {
          _op_code = new __byte[_size];
          _op_code = (__byte *) memcpy(_op_code, op_code,
                                       _size * sizeof(__byte));
        }

      Instruction::Instruction(
          Instruction *_copy)
        {
          _op_code = new __byte[_copy->_size];
          _op_code = (__byte *) memcpy(_op_code, _copy->_op_code,
                                       _copy->_size * sizeof(__byte));
          _s_signature = _copy->_s_signature;
          _address = _copy->_address;
          _binary = _copy->_binary;
          _size = _copy->_size;
          _name = _copy->_name;
        }

      Instruction::__byte *Instruction::getOpCode()
        {
          return _op_code;
        }

      Instruction::__address_t Instruction::getAddress()
        {
          return _address;
        }

      Instruction::__signature_t Instruction::getBinary()
        {
          char _buffer[6];
          if (_binary == "")
            {
              for (size_t i = 0;
                   i < _size;
                   ++i)
                {
                  sprintf(_buffer, "0x%02X ", (unsigned) _op_code[i]);
                  _binary += ::std::string(_buffer);
                }
            }
          return _binary;
        }

      Instruction::~Instruction()
        {
          delete[] _op_code;
        }

      ::std::string Instruction::getName()
        {
          if (_name == "")
            {
              ::std::stringstream ss(_s_signature);
              ss >> _name;
            }
          return _name;
        }

      Instruction::__signature_t Instruction::getSignature()
        {
          return _s_signature;
        }
  }