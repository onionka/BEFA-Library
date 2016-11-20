//
// Created by miro on 11/10/16.
//

#include "../../include/befa.hpp"


void ExecutableFile::runDecompiler() {
  assembly().subscribe([](instruction_type i) {
    // parse instruction 'i'
  });

  runDisassembler();
}