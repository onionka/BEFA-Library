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


namespace bfp
  {
      class Instruction
        {
          friend class Symbol;

          friend class Section;

          friend class Iterator;

          typedef char *c_string;
          typedef sized_raw_vector<
              char,
              32> buffer_t;
      public:
          typedef uint8_t byte;
          typedef uint64_t address_t;
          typedef c_string signature_t;
          typedef byte *pointer;

          /** @return string representation of instruction in Intel style */
          char *getSignature();

          /** @return name of instruction (first word) */
          ::std::string &getName();

          /** @return binary op_code */
          pointer &getOpCode();

          /** @return address of instruction */
          address_t getAddress();

          /** @return string form of binary op_code */
          char *getBinary();

          const LineInfo getLineInfo() const noexcept
            {
              return _get_line();
            }

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
              address_t address);

      public:
          Instruction();

          Instruction(Instruction &&_mv) noexcept;

          Instruction &operator=(Instruction &&_mv) noexcept;

          Instruction(const Instruction &_cp);

          Instruction &operator=(const Instruction &_cp);

          ~Instruction();

      private:
          /** array of bytes */
          pointer _op_code = nullptr;
          signature_t _s_signature;
          address_t _address = 0x0;
          buffer_t _binary;
          size_t _size = 0x0;
          ::std::string _name;
          ::std::function<
              LineInfo()> _get_line;
        };
  }

#endif //__BFP_INSTRUCTION_HPP
