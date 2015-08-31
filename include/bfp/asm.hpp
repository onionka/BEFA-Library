/**
 * @file asm.hpp
 * @author Miroslav Cibulka
 * @brief Instruction class
 */

#ifndef __BFP_INSTRUCTION_HPP
#define __BFP_INSTRUCTION_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif

#include <memory>


namespace bfp
  {
      class Instruction
        {
          friend class Symbol;

          friend class Section;

          friend class Iterator;

          typedef const char *c_string;
      public:
          typedef uint8_t __byte;
          typedef uint64_t __address_t;
          typedef ::std::string __signature_t;
          typedef __byte *__ptr;

          /** @return string representation of instruction in Intel style */
          __signature_t &getSignature();

          /** @return name of instruction (first word) */
          ::std::string &getName();

          /** @return binary op_code */
          __ptr &getOpCode();

          /** @return address of instruction */
          __address_t getAddress();

          /** @return string form of binary op_code */
          __signature_t &getBinary();

      private:
          /**
           * @brief Cannot be created outside this library
           * @param op_code binary representation of instruction
           * @param count of bytes in op_code
           * @param signature is string representation of instruction
           *        like 'mov %eax, %ebx' in intel syntax
           * @param address of instruction (not an offset in file)
           */
          Instruction(
              uint8_t *op_code,
              size_t count,
              const char *signature,
              __address_t address);

      public:
          Instruction();

          Instruction(Instruction &&_mv);

          Instruction &operator=(Instruction &&_mv);

          Instruction(const Instruction &_cp);

          Instruction & operator=(const Instruction &_cp);

          ~Instruction();

      private:
          /** array of bytes */
          __ptr _op_code = nullptr;
          __signature_t _s_signature;
          __address_t _address = 0x0;
          __signature_t _binary;
          size_t _size = 0x0;
          ::std::string _name;
        };
  }

#endif //__BFP_INSTRUCTION_HPP
