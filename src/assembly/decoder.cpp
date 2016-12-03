//
// Created by miro on 12/3/16.
//

#include "../../include/befa/assembly/instruction_decoder.hpp"


namespace symbol_table {

Symbol::temporary_ptr Symbol::perform_operation(
    Symbol::symbol_ptr lhs,
    std::string op,
    Symbol::symbol_ptr rhs
) { return std::make_shared<Temporary>(lhs, op, rhs); }

Symbol::temporary_ptr Symbol::perform_operation(
    std::string op,
    Symbol::symbol_ptr rhs
) { return std::make_shared<Temporary>(op, rhs); }

}  // namespace symbol_table
