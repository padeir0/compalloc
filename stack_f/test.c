#include "stack_f.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 8196
uint8_t buffer[BUFFSIZE];

#define AMOUNTBUFFLETS 16
uint8_t* bufflets[AMOUNTBUFFLETS];

void checkres(enum sf_RES r) {
  if (r != sf_OK) {
    printf("ERROR: %s\n", sf_str_res(r));
    abort();
  }
}

void setbuff(uint8_t* obj, size_t size, uint8_t value) {
  size_t i = 0;
  while (i < size) {
    obj[i] = value;
    i++;
  }
}

bool checkbuff(uint8_t* obj, size_t size, uint8_t value) {
  size_t i = 0;
  while (i < size) {
    if (obj[i] != value) {
      return false;
    }
    i++;
  }
  return true;
}

void test_seq(size_t chunksize) {
  enum sf_RES res;
  uint8_t* bufflet;
  int i = 0;
  stack_f* sf = sf_create(buffer, BUFFSIZE, chunksize, &res);

  i = 0;
  while (i < AMOUNTBUFFLETS) {
    bufflet = sf_alloc(sf);
    if (bufflet == NULL) {
      printf("ERROR: out of memory\n");
      abort();
    }
    setbuff(bufflet, chunksize, i);
    bufflets[i] = bufflet;
    i++;
  }

  i = 0;
  while (i < AMOUNTBUFFLETS) {
    if (checkbuff(bufflets[i], chunksize, i) == false) {
      printf("ERROR: corrupted memory at %d\n", i);
      abort();
    }
    i++;
  }

  i = 0;
  while (i < AMOUNTBUFFLETS) {
    res = sf_free(sf);
    checkres(res);
    i++;
  }

  if (!sf_empty(sf)) {
    printf("ERROR: stack is not empty\n");
    abort();
  }
}

int main() {
  int i = 16;
  while (i < 64) {
    test_seq(i);
    i++;
  }
  printf("OK: test_seq\n");
  return 0;
}
