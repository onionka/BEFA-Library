#include <stdint.h>

int main(int argc, const char **argv) {
  uint64_t temp = 0;
  uint64_t usernb = 3;

  __asm__ volatile (
      "push rax\n"
      "push rcx\n"
      "mov rax, %0 \n"
      "inc rax \n"
      "mov rcx, %1 \n"
      "xor rcx, %1 \n"
      "mov %1, rcx \n"
      "mov rax, %1 \n"
      "pop rcx\n"
      "pop rax\n"
      : // no output
      : "m" (temp),
        "m" (usernb)
  ); // input

  return 0;
}