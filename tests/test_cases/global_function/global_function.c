#include <stdint.h>


int32_t global_function() {
  __asm__ volatile (
      "push rax\n"
      "inc rax \n"
      "pop rcx\n"
  : // no output
  : // no input
  );
  // rax is return
}


int main(int argc, const char **argv) {
  __asm__ volatile ("mov rax, 1");
  return global_function();
}