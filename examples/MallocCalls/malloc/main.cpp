/**
 * @file main.cpp
 * @brief Testing application for MallocCalls
 */

#include <stdlib.h>

static void *memory;
static char *bytes;
static char *byte;

void alloc()
  {
    memory = malloc(100);
  }

void free()
  {
    free(memory);
  }

void _new()
  {
    bytes = new char[50];
    byte = new char();
  }

void _delete()
  {
    delete[] bytes;
    delete byte;
  }

int main()
  {
    alloc();
    free();
    return 0;
  }