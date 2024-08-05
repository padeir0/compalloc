#include "freelist.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef struct {
  uint8_t* ptr;
  size_t size;
} buffer_t;

#define BUFFSIZE 8192
uint8_t buffer[BUFFSIZE];

#define OBJSSIZE 50
buffer_t objs[OBJSSIZE];

int findnonnull() {
  int i = 0;
  while (i < OBJSSIZE) {
    if (objs[i].ptr != NULL) {
      return i;
    }
    i++;
  }
  return -1;
}

int findnull() {
  int i = 0;
  while (i < OBJSSIZE) {
    if (objs[i].ptr == NULL) {
      return i;
    }
    i++;
  }
  return -1;
}

void checkres(enum fl_RES r) {
  if (r != fl_OK) {
    printf("ERROR: %s\n", fl_str_res(r));
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
  enum fl_RES res;
  freelist* fl = fl_create(buffer, BUFFSIZE, &res);
  uint8_t* ptr;
  buffer_t buff;
  uint8_t i;
  size_t size;

  checkres(res);

  i = 0;
  do {
    size = rand()%128;
    ptr = (uint8_t*) fl_alloc(fl, size);
    if (ptr == NULL) {
      printf("ERROR: out of memory (used %ld tried to alloc %ld)\n", fl_used(fl), size);
      abort();
    }
    buff.ptr = ptr;
    buff.size = size;

    setbuff(ptr, size, i);
    objs[i] = buff;
    i++;
  } while (i < OBJSSIZE);

  i = 0;
  while (i < OBJSSIZE) {
    ptr = objs[i].ptr;
    size = objs[i].size;
    if (!checkbuff(ptr, size, i)) {
      printf("ERROR: corrupted memory at %d\n", i);
      abort();
    }
    if (fl_objsize(ptr) != fl_pad(size)) {
      printf("ERROR: corrupted object header at %d, (%ld != %ld)\n", i, fl_objsize(ptr), fl_pad(size));
      abort();
    }
    i++;
  }

  i = 0;
  while (i < OBJSSIZE) {
    res = fl_free(fl, objs[i].ptr);
    if (res != fl_OK) {
      printf("ERROR: %s\n", fl_str_res(res));
      abort();
    }
    i++;
  }

  if (!fl_empty(fl)) {
    printf("ERROR: freelist is not empty (used %ld)\n", fl_used(fl));
    abort();
  }
}

int placeobj(uint8_t* obj, size_t size) {
  int i = findnull();
  if (i < 0) {
    return -1;
  }
  objs[i].ptr = obj;
  objs[i].size = size;
  return i;
}

void _alloc(freelist* fl, size_t objsize) {
  uint8_t* obj;
  enum fl_RES res;
  int i;
  obj = (uint8_t*) fl_alloc(fl, objsize);
  if (obj == NULL) {
    printf("ERROR: out of memory (used %ld)\n", fl_used(fl));
    abort();
  }
  i = placeobj(obj, objsize);
  /* printf("allocating: %d\n", i); */
  if (i < 0) {
    /* printf("out of memory, freeing object 0x%lX\n", (uint64_t)obj); */
    res = fl_free(fl, obj);
    checkres(res);
    return;
  }
  setbuff(obj, objsize, i);
}

void _free(freelist* fl) {
  int i = findnonnull();
  buffer_t buff;
  enum fl_RES res;
  if (i < 0) {
    return;
  }
  buff = objs[i];
  if(checkbuff(buff.ptr, buff.size, i) == false){
    printf("ERROR: corrupted memory at %d\n", i);
    abort();
  }
  /* printf("freeing: %d\n", i); */
  res = fl_free(fl, buff.ptr);
  checkres(res);
  objs[i].ptr = NULL;
  objs[i].size = 0;
}

void test_mixed() {
  size_t objsize;
  enum fl_RES res;
  freelist* fl = fl_create(buffer, BUFFSIZE, &res);
  int i;
  bzero(objs, sizeof(objs));

  checkres(res);

  i = 0;
  do {
    objsize = rand() % 128;
    if (rand() % 2 == 0) {
      _alloc(fl, objsize);
    } else {
      _free(fl);
    }
    i++;
  } while (i < 10000);

  i = 0;
  while (i < OBJSSIZE) {
    if (objs[i].ptr != NULL){
      res = fl_free(fl, objs[i].ptr);
      if (res != fl_OK) {
        printf("ERROR: %s\n", fl_str_res(res));
        abort();
      }
    }
    i++;
  }

  if (!fl_empty(fl)) {
    printf("ERROR: pool is not empty\n");
    abort();
  }
}

int main() {
  size_t i = 16;
  while (i<64) {
    test_seq();
    i++;
  }
  printf("OK: test_seq\n");

  i = 16;
  while (i<64) {
    test_mixed();
    i++;
  }
  printf("OK: test_mixed\n");
  return 0;
}
