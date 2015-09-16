/**
 * @file main.cpp
 * @author Miroslav Cibulka
 * @brief This is easy disassembler using BFP library
 */

#include <iostream>
#include <bfp.hpp>


int main(
    int argc,
    const char *argv[])
  {
    try
      {
        if (argc == 1) throw ::std::runtime_error("File argument needed");
        auto parser = ::bfp::Parser::get_unique_instance();
        for (::bfp::Section &sec : parser.Open(argv[1], ""))
          {
            printf("%s <0x%08X>:\n", sec.getName()
                                        .c_str(), (unsigned) sec.getAddress());
            for (::bfp::Symbol &sym : sec)
              {
                printf("    %s <0x%08X>:\n", sym.getName()
                                                .c_str(),
                       (unsigned) sym.getValue());
                if (sec.hasCodeOnly())
                  for (::bfp::Instruction &_instr : sym)
                      printf("        <0x%08X>: %-80.80s %s\n",
                             (unsigned) _instr.getAddress(),
                             _instr.getSignature(), _instr.getBinary());
              }
          }
      }
    catch (::std::exception &e)
      {
        ::std::cerr << e.what() << ::std::endl;
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }
