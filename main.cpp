#include <befa.hpp>


int main(int argc, const char **argv) {

  assert(argc == 2 && "missing path parameter");


  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));
  auto &o$ = file.assembly();

  assert(file.isValid() && "file is not valid");

  o$.subscribe([] (const ExecutableFile::instruction_type &i) {
    printf("\t%s\n", i.getDecoded().c_str());
  });

  file.runDisassembler();

  return 0;
}