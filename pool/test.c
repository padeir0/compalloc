#include "pool.h"
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 8192
uint8_t buffer[BUFFSIZE];

#define OBJSSIZE 100
uint8_t* objs[OBJSSIZE];

void checkres(enum RES r) {
  if (r != OK) {
    printf("ERROR: %s\n", str_res(r));
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

void test_seq(size_t objsize) {
  enum RES res;
  pool* p = pool_create(buffer, BUFFSIZE, objsize, &res);
  uint8_t* obj;
  uint8_t i;

  checkres(res);

  i = 0;
  do {
    obj = (uint8_t*) pool_alloc(p);
    if (obj == NULL) {
      printf("ERROR: out of memory (used %ld)\n", pool_used(p));
      abort();
    }
    setbuff(obj, objsize, i);
    objs[i] = obj;
    i++;
  } while (i < OBJSSIZE);

  i = 0;
  while (i < OBJSSIZE) {
    if (!checkbuff(objs[i], objsize, i)) {
      printf("ERROR: corrupted memory at %d\n", i);
      abort();
    }
    i++;
  }

  i = 0;
  while (i < OBJSSIZE) {
    res = pool_free(p, objs[i]);
    if (res != OK) {
      printf("ERROR: %s\n", str_res(res));
      abort();
    }
    i++;
  }

  if (!pool_empty(p)) {
    printf("ERROR: pool is not empty\n");
    abort();
  }
}

int findnonnull() {
  int i = 0;
  while (i < OBJSSIZE) {
    if (objs[i] != NULL) {
      return i;
    }
    i++;
  }
  return -1;
}

int findnull() {
  int i = 0;
  while (i < OBJSSIZE) {
    if (objs[i] == NULL) {
      return i;
    }
    i++;
  }
  return -1;
}

int placeobj(uint8_t* obj) {
  int i = findnull();
  if (i < 0) {
    return -1;
  }
  objs[i] = obj;
  return i;
}

void _alloc(pool* p, size_t objsize) {
  uint8_t* obj;
  enum RES res;
  int i;
  obj = (uint8_t*) pool_alloc(p);
  if (obj == NULL) {
    printf("ERROR: out of memory (used %ld)\n", pool_used(p));
    abort();
  }
  i = placeobj(obj);
  /* printf("allocating: %d\n", i); */
  if (i < 0) {
    /* printf("out of memory, freeing object 0x%lX\n", (uint64_t)obj); */
    res = pool_free(p, obj);
    checkres(res);
    return;
  }
  setbuff(obj, objsize, i);
}

void _free(pool* p, size_t objsize) {
  int i = findnonnull();
  uint8_t* obj;
  enum RES res;
  if (i < 0) {
    return;
  }
  obj = objs[i];
  if(checkbuff(obj, objsize, i) == false){
    printf("ERROR: corrupted memory at %d\n", i);
    abort();
  }
  /* printf("freeing: %d\n", i); */
  res = pool_free(p, obj);
  checkres(res);
  objs[i] = NULL;
}

void test_mixed(size_t objsize) {
  enum RES res;
  pool* p = pool_create(buffer, BUFFSIZE, objsize, &res);
  int i;
  bzero(objs, sizeof(objs));

  checkres(res);

  i = 0;
  do {
    if (rand() % 2 == 0) {
      _alloc(p, objsize);
    } else {
      _free(p, objsize);
    }
    i++;
  } while (i < 10000);

  i = 0;
  while (i < OBJSSIZE) {
    if (objs[i] != NULL){
      res = pool_free(p, objs[i]);
      if (res != OK) {
        printf("ERROR: %s\n", str_res(res));
        abort();
      }
    }
    i++;
  }

  if (!pool_empty(p)) {
    printf("ERROR: pool is not empty\n");
    abort();
  }
}

int main() {
  size_t i = 16;
  while (i<64) {
    test_seq(i);
    i++;
  }
  printf("OK: test_seq\n");

  i = 16;
  while (i<64) {
    test_mixed(i);
    i++;
  }
  printf("OK: test_mixed\n");
  return 0;
}
