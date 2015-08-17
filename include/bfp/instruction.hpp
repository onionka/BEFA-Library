//
// Created by onionka on 15/08/15.
//

#ifndef BINARYFILEPARSER_INSTRUCTION_H
#define BINARYFILEPARSER_INSTRUCTION_H

#include <sstream>


namespace bfp
  {
      class Instruction
        {
          friend class Symbol;

          typedef const char *c_string;
      public:
          typedef uint8_t __index_t;
          typedef uint8_t __opcode_t;
          typedef uint8_t *__opcode_ptr;
          typedef uint64_t __address_t;
          typedef ::std::string __signature_t;

          /**
           * @return string representation of instruction in Intel style
           */
          __signature_t getSignature()
            {
              return _s_signature;
            }

          /**
           * @return name of instruction (first word)
           */
          __signature_t getName()
            {
              if (_name == "")
                {
                  ::std::stringstream ss(_s_signature);
                  ss >> _name;
                }
              return _name;
            }

          __opcode_ptr getOpCode()
            {
              return _op_code;
            }

          __address_t getAddress()
            {
              return _address;
            }

          __signature_t getBinary()
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

          ~Instruction()
            {
              delete[] _op_code;
            }

      private:
          Instruction(
              uint8_t *op_code,
              size_t count,
              const char *signature,
              __address_t address)
              :
              _s_signature(signature),
              _address(address),
              _size(count),
              _name("")
            {
              _op_code = new __opcode_t[_size];
              _op_code = (__opcode_ptr) memcpy(_op_code, op_code,
                                               _size * sizeof(__opcode_t));
            }

          Instruction(
              const Instruction *_copy)
            {
              _op_code = new __opcode_t[_copy->_size];
              _op_code = (__opcode_ptr) memcpy(_op_code, _copy->_op_code,
                                               _copy->_size * sizeof(__opcode_t));
              _s_signature = _copy->_s_signature;
              _address = _copy->_address;
              _binary = _copy->_binary;
              _size = _copy->_size;
              _name = _copy->_name;
            }

          /** Forbidden primitive constructor */
          Instruction() = delete;

          /** Forbidden copy constructor */
          Instruction(const Instruction &) = delete;

          /** Forbidden move constructor */
          Instruction(Instruction &&) = delete;

          /** Forbidden copy assignment */
          Instruction &operator=(const Instruction &) = delete;

          /** Forbidden move assignment */
          Instruction &operator=(Instruction &&) = delete;

      private:
          __opcode_ptr _op_code;
          __signature_t _s_signature;
          __address_t _address;
          __signature_t _binary;
          size_t _size;
          ::std::string _name;
        };
  }

#endif //BINARYFILEPARSER_INSTRUCTION_H
