# Library for Binary Executable File Analysis 

## What is done

 - [X] Disassembling binary file into instruction stream
 - [X] Creating syntactic tree from instruction parameters (ie. pointer arithmetic, ...)
 - [X] Boilerplate for implementing mappers from assembly instructions into LLVM IR
 - [ ] Implemented core LLVM Instructions (useful for static analysis):
    - [X] [call instruction](http://llvm.org/doxygen/classllvm_1_1CallInst.html)
    - [X] [binary operator](http://llvm.org/doxygen/classllvm_1_1BinaryOperator.html)
    - [X] [compare instruction](http://llvm.org/doxygen/classllvm_1_1CmpInst.html)
    - [ ] [terminator instructions](http://llvm.org/doxygen/classllvm_1_1TerminatorInst.html)
       - [ ] [branch instruction](http://llvm.org/doxygen/classllvm_1_1BranchInst.html)
    - [ ] [unary instruction](http://llvm.org/doxygen/classllvm_1_1UnaryInstruction.html)
 
## Requirements:
 - binutils-dev >= 2.2
 - cmake >= 3.2
 - g++ >= 6.0
 - pcrecpp
 - gtest
 - doxygen (doc)

## Building:
```
 $ mkdir build && pushd build
 $ cmake .. --DCMAKE_BUILD_TYPE=Release
 $ make -j4
 $ make test
 $ popd
```

## Project structure:
 - examples
 - include
   - befa
     - assembly
     - llvm
     - utils
 - src
   - assembly
   - llvm
 - tests
   - test_cases


### Example

Here is disassembler using this library, that prints an instructions
with decoded types, addresses to symbols etc.:

```c++
#include <befa.hpp>


// ExecutableFile types we'll be using
using instr_type = ExecutableFile::inst_t::type;
using basic_block_type = ExecutableFile::bb_t::type;
using symbol_type = ExecutableFile::sym_t::type;
using section_type = ExecutableFile::sec_t::type;

int main(int argc, const char **argv) {
  // check if first param is not missing
  assert_ex(argc == 2, "missing path parameter");

  // open file
  auto file = ExecutableFile::open(*(argv + 1));

  // check if file is valid
  assert_ex(file.isValid(), "file is not valid");

  // generate symbol table
  auto sym_table_symbols = file.generate_table();

  file.disassembly()
          // RxCpp::reduce(...) function
      .reduce(
          // default last basic block,         symbol,         section
          std::make_tuple((bfd_vma)-1, std::string(""), std::string("")),
          [&sym_table_symbols](
              auto acc,
              const auto &instruction
          ) {
            auto bb = instruction.getParent();
            auto sym = ptr_lock(bb->getParent());
            auto sec = ptr_lock(sym->getParent());

            auto last_sec = sec->getName();
            auto last_sym = sym->getName();
            auto last_bb = bb->getId();

            if (std::get<2>(acc) != last_sec) {
              printf("%s <0x%08lx>:\n",
                     sec->getName().c_str(),
                     sec->getAddress()
              );
            }
            if (std::get<1>(acc) != last_sym) {
              printf("  %s <0x%08lx>:\n",
                     sym->getName().c_str(),
                     sym->getAddress()
              );
            }
            if (std::get<0>(acc) != last_bb) {
              printf("    BasicBlock #%lu <0x%08lx>:\n",
                     bb->getId(),
                     instruction.getAddress()
              );
            }

                // iterate through arguments of instruction
            instruction
                .getArgs(sym_table_symbols)

                    // convert to string
                .map([](
                    auto arg
                ) {
                  // try to use 'arg' as a Symbol
                  // if not, default value "" will be mapped instead
                  return map_visitable<symbol_table::SymbolVisitorL>(
                      arg, [](const symbol_table::Symbol *ptr)
                            { return ptr->getName(); }
                  );
                })

                    // filter out empty (non-symbol stuff)
                .filter([](
                    auto name
                ) { return name != ""; })

                    // string join achieved by reduction
                .reduce(std::string(""), [](
                    auto seed, auto b
                ) { return seed == "" ? b : seed + ", " + b; })

                    // print instruction
                .subscribe([&instruction](
                    auto str_params
                ) {
                  printf("      <0x%08lx> %s %s\n",
                         instruction.getAddress(),
                         instruction.getName().c_str(),
                         str_params.c_str());
                });

            std::get<2>(acc) = last_sec;
            std::get<1>(acc) = last_sym;
            std::get<0>(acc) = last_bb;

            return acc;
          }  // lambda::reduce()
          // we don't care about result of reduction
          // output we need is in the console
      ).subscribe([](const auto &) {});

  // executes all
  file.runDisassembler();
  return 0;
}
```

### Output of example

This is output of disassembler, spiced with a little type deduction and decoded
symbol addresses:
```
~/Work/BEFA-2/build/examples/disassembler/disassembler ../../tests/c_prog/sample
.comment <0x00000000>:
  .comment <0x00000000>:
    BasicBlock #0 <0x00000000>:
      <0x00000000> rex.RXB 
      <0x00000001> rex.XB 
      <0x00000002> rex.XB ((BYTE)*(((QWORD)_r8)))
      <0x00000005> sub ((BYTE)*(((QWORD)_rdi)+0x4e)), ((BYTE)_al)
      <0x00000008> push ((QWORD)_rbp)
      <0x00000009> sub ((DWORD)*(((QWORD)_rax))), ((DWORD)_esp)
      <0x0000000b> ss ((BYTE)_ch), ((BYTE)*(((QWORD)_rsi)))
      <0x0000000f> xor ((DWORD)*(((QWORD)_rax))), ((DWORD)_esp)
      <0x00000011> xor ((BYTE)_dh), ((BYTE)*(((QWORD)_rax)))
      <0x00000013> xor ((DWORD)*(((QWORD)_rsi))), ((DWORD)_esi)
      <0x00000015> xor ((BYTE)*(((QWORD)_rcx))), ((BYTE)_bh)
      <0x00000017> xor ((DWORD)*(((QWORD)_rsi))), ((DWORD)_esi)
      <0x00000019> and ((BYTE)*(((QWORD)_rax))), ((BYTE)_ch)
      <0x0000001b> push ((QWORD)_rdx)
      <0x0000001c> gs ((BYTE)*(((QWORD)_rax)+0x61)), ((BYTE)_cl)
      <0x00000021> je 0000000000000043
    BasicBlock #35 <0x00000023>:
      <0x00000023> ss ((BYTE)_ch), ((BYTE)*(((QWORD)_rsi)))
      <0x00000027> .byte 31
      <0x00000028> .byte 2d
      <0x00000029> xor ((BYTE)_ch), ((BYTE)*(((QWORD)_rcx)))
      <0x0000002b> .byte @.comment
.init <0x004007d0>:
  .init <0x004007d0>:
    BasicBlock #4196304 <0x004007d0>:
      <0x004007d0> sub ((QWORD)_rsp), 8
      <0x004007d4> mov ((QWORD)_rax), ((QWORD)*(((QWORD)_rip)+0x20081d))
      <0x004007db> test ((QWORD)_rax), ((QWORD)_rax)
      <0x004007de> je 00000000004007e5
    BasicBlock #4196320 <0x004007e0>:
      <0x004007e0> call @.plt.got
    BasicBlock #4196325 <0x004007e5>:
      <0x004007e5> add ((QWORD)_rsp), 8
      <0x004007e9> ret 
      <0x00400800> jmp ((QWORD)*(((QWORD)_rip)+0x200812))
      <0x00400806> push @.comment
      <0x0040080b> jmp @.plt
      <0x00400810> jmp ((QWORD)*(((QWORD)_rip)+0x20080a))
      <0x00400816> push 1
      <0x0040081b> jmp @.plt
.text <0x00400830>:
  .text <0x00400830>:
    BasicBlock #4196400 <0x00400830>:
      <0x00400830> xor ((DWORD)_ebp), ((DWORD)_ebp)
      <0x00400832> mov ((QWORD)_r9), ((QWORD)_rdx)
      <0x00400835> pop ((QWORD)_rsi)
      <0x00400836> mov ((QWORD)_rdx), ((QWORD)_rsp)
      <0x00400839> and ((QWORD)_rsp), fffffffffffffff0
      <0x0040083d> push ((QWORD)_rax)
      <0x0040083e> push ((QWORD)_rsp)
      <0x0040083f> mov ((QWORD)_r8), @__libc_csu_fini
      <0x00400846> mov ((QWORD)_rcx), @__libc_csu_init
      <0x0040084d> mov ((QWORD)_rdi), @main
      <0x00400854> call @__libc_start_main@plt
      <0x00400859> hlt 
      <0x0040085a> nop ((WORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
  deregister_tm_clones <0x00400860>:
    BasicBlock #4196448 <0x00400860>:
      <0x00400860> mov ((DWORD)_eax), 601077
      <0x00400865> push ((QWORD)_rbp)
      <0x00400866> sub ((QWORD)_rax), @.bss
      <0x0040086c> cmp ((QWORD)_rax), e
      <0x00400870> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x00400873> jbe 0000000000400890
    BasicBlock #4196469 <0x00400875>:
      <0x00400875> mov ((DWORD)_eax), @.comment
      <0x0040087a> test ((QWORD)_rax), ((QWORD)_rax)
      <0x0040087d> je 0000000000400890
    BasicBlock #4196479 <0x0040087f>:
      <0x0040087f> pop ((QWORD)_rbp)
      <0x00400880> mov ((DWORD)_edi), @.bss
      <0x00400885> jmp ((QWORD)_rax)
    BasicBlock #4196487 <0x00400887>:
      <0x00400887> nop ((WORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
    BasicBlock #4196496 <0x00400890>:
      <0x00400890> pop ((QWORD)_rbp)
      <0x00400891> ret 
      <0x00400892> nop ((DWORD)*(((QWORD)_rax)+@.comment))
      <0x00400896> nop ((WORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
  register_tm_clones <0x004008a0>:
    BasicBlock #4196512 <0x004008a0>:
      <0x004008a0> mov ((DWORD)_esi), @.bss
      <0x004008a5> push ((QWORD)_rbp)
      <0x004008a6> sub ((QWORD)_rsi), @.bss
      <0x004008ad> sar ((QWORD)_rsi), 3
      <0x004008b1> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x004008b4> mov ((QWORD)_rax), ((QWORD)_rsi)
      <0x004008b7> shr ((QWORD)_rax), 3f
      <0x004008bb> add ((QWORD)_rsi), ((QWORD)_rax)
      <0x004008be> sar ((QWORD)_rsi), 1
      <0x004008c1> je 00000000004008d8
    BasicBlock #4196547 <0x004008c3>:
      <0x004008c3> mov ((DWORD)_eax), @.comment
      <0x004008c8> test ((QWORD)_rax), ((QWORD)_rax)
      <0x004008cb> je 00000000004008d8
    BasicBlock #4196557 <0x004008cd>:
      <0x004008cd> pop ((QWORD)_rbp)
      <0x004008ce> mov ((DWORD)_edi), @.bss
      <0x004008d3> jmp ((QWORD)_rax)
    BasicBlock #4196565 <0x004008d5>:
      <0x004008d5> nop ((DWORD)*(((QWORD)_rax)))
    BasicBlock #4196568 <0x004008d8>:
      <0x004008d8> pop ((QWORD)_rbp)
      <0x004008d9> ret 
      <0x004008da> nop ((WORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
  __do_global_dtors_aux <0x004008e0>:
    BasicBlock #4196576 <0x004008e0>:
      <0x004008e0> cmp ((BYTE)*(((QWORD)_rip)+0x200789)), @.comment
      <0x004008e7> jne 00000000004008fa
    BasicBlock #4196585 <0x004008e9>:
      <0x004008e9> push ((QWORD)_rbp)
      <0x004008ea> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x004008ed> call @deregister_tm_clones
      <0x004008f2> pop ((QWORD)_rbp)
      <0x004008f3> mov ((BYTE)*(((QWORD)_rip)+0x200776)), 1
    BasicBlock #4196602 <0x004008fa>:
      <0x004008fa> repz 
      <0x004008fc> nop ((DWORD)*(((QWORD)_rax)+@.comment))
      <0x00400900> mov ((DWORD)_edi), @.jcr
      <0x00400905> cmp ((QWORD)*(((QWORD)_rdi))), @.comment
      <0x00400909> jne 0000000000400910
      <0x0040090b> jmp @register_tm_clones
      <0x0040090d> nop ((DWORD)*(((QWORD)_rax)))
      <0x00400910> mov ((DWORD)_eax), @.comment
      <0x00400915> test ((QWORD)_rax), ((QWORD)_rax)
      <0x00400918> je 000000000040090b
      <0x0040091a> push ((QWORD)_rbp)
      <0x0040091b> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x0040091e> call ((QWORD)_rax)
      <0x00400920> pop ((QWORD)_rbp)
      <0x00400921> jmp @register_tm_clones
  no_return <0x00400926>:
    BasicBlock #4196646 <0x00400926>:
      <0x00400926> push ((QWORD)_rbp)
      <0x00400927> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x0040092a> nop 
      <0x0040092b> pop ((QWORD)_rbp)
      <0x0040092c> ret 
  return_numeric <0x0040092d>:
    BasicBlock #4196653 <0x0040092d>:
      <0x0040092d> push ((QWORD)_rbp)
      <0x0040092e> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x00400931> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rip)+0x200709))
      <0x00400937> pop ((QWORD)_rbp)
      <0x00400938> ret 
  return_address <0x00400939>:
    BasicBlock #4196665 <0x00400939>:
      <0x00400939> push ((QWORD)_rbp)
      <0x0040093a> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x0040093d> mov ((DWORD)_eax), @numeric
      <0x00400942> pop ((QWORD)_rbp)
      <0x00400943> ret 
  algorithm <0x00400944>:
    BasicBlock #4196676 <0x00400944>:
      <0x00400944> push ((QWORD)_rbp)
      <0x00400945> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x00400948> sub ((QWORD)_rsp), 10
      <0x0040094c> mov ((DWORD)*(((QWORD)_rbp)-0x4)), ((DWORD)_edi)
      <0x0040094f> cmp ((DWORD)*(((QWORD)_rbp)-0x4)), @.comment
      <0x00400953> jne 000000000040095a
    BasicBlock #4196693 <0x00400955>:
      <0x00400955> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0x4))
      <0x00400958> jmp 0000000000400975
    BasicBlock #4196698 <0x0040095a>:
      <0x0040095a> cmp ((DWORD)*(((QWORD)_rbp)-0x4)), @.comment
      <0x0040095e> jns 0000000000400968
    BasicBlock #4196704 <0x00400960>:
      <0x00400960> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0x4))
      <0x00400963> add ((DWORD)_eax), 1
      <0x00400966> jmp 000000000040096e
    BasicBlock #4196712 <0x00400968>:
      <0x00400968> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0x4))
      <0x0040096b> sub ((DWORD)_eax), 1
    BasicBlock #4196718 <0x0040096e>:
      <0x0040096e> mov ((DWORD)_edi), ((DWORD)_eax)
      <0x00400970> call @algorithm
    BasicBlock #4196725 <0x00400975>:
      <0x00400975> leave 
      <0x00400976> ret 
  function_with_locals <0x00400977>:
    BasicBlock #4196727 <0x00400977>:
      <0x00400977> push ((QWORD)_rbp)
      <0x00400978> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x0040097b> mov ((DWORD)_edx), ((DWORD)*(((QWORD)_rbp)-0x4))
      <0x0040097e> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0x8))
      <0x00400981> add ((DWORD)_edx), ((DWORD)_eax)
      <0x00400983> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0xc))
      <0x00400986> add ((DWORD)_edx), ((DWORD)_eax)
      <0x00400988> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0x10))
      <0x0040098b> add ((DWORD)_eax), ((DWORD)_edx)
      <0x0040098d> mov ((DWORD)*(((QWORD)_rip)+0x2006ad)), ((DWORD)_eax)
      <0x00400993> nop 
      <0x00400994> pop ((QWORD)_rbp)
      <0x00400995> ret 
  main <0x00400996>:
    BasicBlock #4196758 <0x00400996>:
      <0x00400996> push ((QWORD)_rbp)
      <0x00400997> mov ((QWORD)_rbp), ((QWORD)_rsp)
      <0x0040099a> sub ((QWORD)_rsp), 20
      <0x0040099e> mov ((DWORD)*(((QWORD)_rbp)-0x14)), ((DWORD)_edi)
      <0x004009a1> movsd ((XMMWORD)_xmm0), ((QWORD)*(((QWORD)_rip)+0xdf))
      <0x004009a9> movsd ((QWORD)*(((QWORD)_rbp)-0x8)), ((XMMWORD)_xmm0)
      <0x004009ae> movsd ((XMMWORD)_xmm0), ((QWORD)*(((QWORD)_rbp)-0x8))
      <0x004009b3> mulsd ((XMMWORD)_xmm0), ((QWORD)*(((QWORD)_rbp)-0x8))
      <0x004009b8> movsd ((QWORD)*(((QWORD)_rbp)-0x8)), ((XMMWORD)_xmm0)
      <0x004009bd> mov ((QWORD)_rax), ((QWORD)*(((QWORD)_rbp)-0x8))
      <0x004009c1> mov ((QWORD)*(((QWORD)_rbp)-0x20)), ((QWORD)_rax)
      <0x004009c5> movsd ((XMMWORD)_xmm0), ((QWORD)*(((QWORD)_rbp)-0x20))
      <0x004009ca> mov ((DWORD)_edi), 400a80
      <0x004009cf> mov ((DWORD)_eax), 1
      <0x004009d4> call @printf@plt
      <0x004009d9> mov ((DWORD)_eax), ((DWORD)*(((QWORD)_rbp)-0x14))
      <0x004009dc> mov ((DWORD)_edi), ((DWORD)_eax)
      <0x004009de> call @algorithm
      <0x004009e3> leave 
      <0x004009e4> ret 
      <0x004009e5> nop ((WORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
      <0x004009ef> nop 
  __libc_csu_init <0x004009f0>:
    BasicBlock #4196848 <0x004009f0>:
      <0x004009f0> push ((QWORD)_r15)
      <0x004009f2> push ((QWORD)_r14)
      <0x004009f4> mov ((DWORD)_r15d), ((DWORD)_edi)
      <0x004009f7> push ((QWORD)_r13)
      <0x004009f9> push ((QWORD)_r12)
      <0x004009fb> lea ((QWORD)_r12), ((QWORD)_rip)+0x20040e
      <0x00400a02> push ((QWORD)_rbp)
      <0x00400a03> lea ((QWORD)_rbp), ((QWORD)_rip)+0x20040e
      <0x00400a0a> push ((QWORD)_rbx)
      <0x00400a0b> mov ((QWORD)_r14), ((QWORD)_rsi)
      <0x00400a0e> mov ((QWORD)_r13), ((QWORD)_rdx)
      <0x00400a11> sub ((QWORD)_rbp), ((QWORD)_r12)
      <0x00400a14> sub ((QWORD)_rsp), 8
      <0x00400a18> sar ((QWORD)_rbp), 3
      <0x00400a1c> call @.init
      <0x00400a21> test ((QWORD)_rbp), ((QWORD)_rbp)
      <0x00400a24> je 0000000000400a46
    BasicBlock #4196902 <0x00400a26>:
      <0x00400a26> xor ((DWORD)_ebx), ((DWORD)_ebx)
      <0x00400a28> nop ((DWORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
    BasicBlock #4196912 <0x00400a30>:
      <0x00400a30> mov ((QWORD)_rdx), ((QWORD)_r13)
      <0x00400a33> mov ((QWORD)_rsi), ((QWORD)_r14)
      <0x00400a36> mov ((DWORD)_edi), ((DWORD)_r15d)
      <0x00400a39> call ((QWORD)*(((QWORD)_r12)+((QWORD)_rbx)*8))
      <0x00400a3d> add ((QWORD)_rbx), 1
      <0x00400a41> cmp ((QWORD)_rbp), ((QWORD)_rbx)
      <0x00400a44> jne 0000000000400a30
    BasicBlock #4196934 <0x00400a46>:
      <0x00400a46> add ((QWORD)_rsp), 8
      <0x00400a4a> pop ((QWORD)_rbx)
      <0x00400a4b> pop ((QWORD)_rbp)
      <0x00400a4c> pop ((QWORD)_r12)
      <0x00400a4e> pop ((QWORD)_r13)
      <0x00400a50> pop ((QWORD)_r14)
      <0x00400a52> pop ((QWORD)_r15)
      <0x00400a54> ret 
      <0x00400a55> nop 
      <0x00400a56> nop ((WORD)*(((QWORD)_rax)+((QWORD)_rax)*1+@.comment))
  __libc_csu_fini <0x00400a60>:
    BasicBlock #4196960 <0x00400a60>:
      <0x00400a60> repz 
.fini <0x00400a64>:
  .fini <0x00400a64>:
    BasicBlock #4196964 <0x00400a64>:
      <0x00400a64> sub ((QWORD)_rsp), 8
      <0x00400a68> add ((QWORD)_rsp), 8
      <0x00400a6c> ret 

Process finished with exit code 0
```
