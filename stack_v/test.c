#include "stack_v.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 8196
uint8_t buffer[BUFFSIZE];

#define AMOUNTBUFFLETS 16
uint8_t* bufflets[AMOUNTBUFFLETS];

void checkres(enum sv_RES r) {
  if (r != sv_OK) {
    printf("ERROR: %s\n", sv_str_res(r));
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

void test_seq() {
  size_t minchunksize = sizeof(void*);
  size_t chunksize;
  enum sv_RES res;
  uint8_t* bufflet;
  int i = 0;
  stack_v* sv = sv_create(buffer, BUFFSIZE, &res);

  i = 0;
  while (i < AMOUNTBUFFLETS) {
    chunksize = minchunksize + rand()%128;
    bufflet = sv_alloc(sv, chunksize);
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
    if (checkbuff(bufflets[i], minchunksize, i) == false) {
      printf("ERROR: corrupted memory at %d\n", i);
      abort();
    }
    i++;
  }

  i = 0;
  while (i < AMOUNTBUFFLETS) {
    res = sv_free(sv);
    checkres(res);
    i++;
  }

  if (!sv_empty(sv)) {
    printf("ERROR: stack is not empty\n");
    abort();
  }
}

int main() {
  int i = 16;
  while (i < 64) {
    test_seq();
    i++;
  }
  printf("OK: test_seq\n");
  return 0;
}
