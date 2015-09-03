//
// Created by onionka on 15/08/15.
//

#include "../include/bfp.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>


namespace bfp
  {
      Instruction::Instruction(
          uint8_t *op_code,
          size_t count,
          const char *signature,
          Instruction::address_t address)
          :
          _op_code(op_code),
          _s_signature(signature, strlen(signature) + 1),
          _address(address),
          _size(count),
          _name("")
        { }

      Instruction::pointer &Instruction::getOpCode()
        {
          return _op_code;
        }

      Instruction::address_t Instruction::getAddress()
        {
          return _address;
        }

      char *Instruction::getBinary()
        {
          static const char *_hex_numbers = "0123456789ABCDEF";
          _binary.resize(_size * 3 + 1);
          for (size_t n = 0;
               n < _size;
               ++n)
            {
              ((char *) _binary)[n * 3] = _hex_numbers[_op_code[n] &
                                                       (uint8_t) 0x0F];
              ((char *) _binary)[n * 3 + 1] = _hex_numbers[(_op_code[n] >> 4) &
                                                           (uint8_t) 0x0F];
              ((char *) _binary)[n * 3 + 2] = ' ';
            };
          ((char *) _binary)[_size * 3] = '\0';
          return _binary;
        }

      ::std::string &Instruction::getName()
        {
          if (_name == "")
            {
              ::std::string _str(_s_signature);
              ::std::stringstream ss(_str);
              ss >> _name;
            }
          return _name;
        }

      char *Instruction::getSignature()
        {
          return _s_signature;
        }

      Instruction::Instruction(Instruction &&_mv)
        {
          _op_code = _mv._op_code;
          _s_signature = ::std::move(_mv._s_signature);
          _address = _mv._address;
          _binary = ::std::move(_mv._binary);
          _size = _mv._size;
          _name = ::std::move(_mv._name);
        }

      Instruction &Instruction::operator=(Instruction &&_mv)
        {
          _op_code = _mv._op_code;
          _s_signature = ::std::move(_mv._s_signature);
          _address = _mv._address;
          _binary = ::std::move(_mv._binary);
          _size = _mv._size;
          _name = ::std::move(_mv._name);
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