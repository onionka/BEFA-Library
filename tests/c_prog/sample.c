//
// Created by miro on 18/10/16.
//


volatile int numeric = 1;
volatile double floating = 0.1f;
volatile const char string[] = "string";
volatile int array[] = {0, 1, 2, 3};


void no_return() {}

volatile int return_numeric() { return numeric; }

volatile int *return_address() { return &numeric; }

int algorithm(int i) {
  if (i == 0)
    return i;
  else
    return algorithm((i < 0) ? i + 1 : i - 1);
}


void function_with_locals() {
  volatile int a, b, c, d;
  numeric = a + b + c + d;
}


int main(int argc) {
  return algorithm(argc);
}