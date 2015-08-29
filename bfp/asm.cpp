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
          _op_code(op_code),
          _s_signature(signature),
          _address(address),
          _size(count),
          _name("")
        { }

      Instruction::__ptr &Instruction::getOpCode()
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

      Instruction::Instruction(Instruction &&_mv)
        {
          _op_code = _mv._op_code;
          _s_signature = _mv._s_signature;
          _address = _mv._address;
          _binary = _mv._binary;
          _size = _mv._size;
          _name = _mv._name;
        }

      Instruction &Instruction::operator=(Instruction &&_mv)
        {
          _op_code = _mv._op_code;
          _s_signature = _mv._s_signature;
          _address = _mv._address;
          _binary = _mv._binary;
          _size = _mv._size;
          _name = _mv._name;
          return *this;
        }

      Instruction::Instruction(const Instruction &_cp)
        {
          _op_code = _cp._op_code;
          _s_signature = _cp._s_signature;
          _address = _cp._address;
          _binary = _cp._binary;
          _size = _cp._size;
          _name = _cp._name;
        }

      Instruction &Instruction::operator=(const Instruction &_cp)
        {
          _op_code = _cp._op_code;
          _s_signature = _cp._s_signature;
          _address = _cp._address;
          _binary = _cp._binary;
          _size = _cp._size;
          _name = _cp._name;
          return *this;
        }

      Instruction::~Instruction()
        { }

      Instruction::Instruction()
        { }
  }