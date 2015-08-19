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

          typedef const char *c_string;
      public:
          typedef uint8_t __byte;
          typedef uint64_t __address_t;
          typedef ::std::string __signature_t;

          /** @return string representation of instruction in Intel style */
          __signature_t getSignature();

          /** @return name of instruction (first word) */
          ::std::string getName();

          /** @return binary op_code */
          __byte *getOpCode();

          /** @return address of instruction */
          __address_t getAddress();

          /** @return string form of binary op_code */
          __signature_t getBinary();

      private:
          /** frees memory */
          ~Instruction();

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

          /**
           * @brief Copies Instruction object - internal use
           * @param _copy object that will be copied
           */
          Instruction(
              Instruction *_copy);

          void realloc(size_t _s)
            {
              if (_size <= _s)
                {
                  delete[] _op_code;
                  _op_code = new __byte[_s];
                }
            }

      public:
          Instruction()
            {
              _op_code = new __byte[0];
              _s_signature = "";
              _address = 0x0;
              _binary = "";
              _size = 0;
              _name = "";
            }
      private:

          /** Forbidden copy constructor */
          Instruction(const Instruction &) = delete;

          /** Forbidden move constructor */
          Instruction(Instruction &&) = delete;

          /** Forbidden copy assignment */
          Instruction &operator=(const Instruction &) = delete;

          /** Forbidden move assignment */
          Instruction &operator=(Instruction &&) = delete;

      private:
          /** array of bytes */
          __byte *_op_code;
          __signature_t _s_signature;
          __address_t _address;
          __signature_t _binary;
          size_t _size;
          ::std::string _name;
        };
  }

#endif //__BFP_INSTRUCTION_HPP
