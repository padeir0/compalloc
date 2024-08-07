#include "stack_f.h"

char* sf_str_res(enum sf_RES res) {
  switch (res) {
    case sf_OK:
      return "OK";
    case sf_SMALLBUFF:
      return "Provided buffer is too small";
    case sf_STACKEMPTY:
      return "Stack is empty";
  }
  return "???";
}

stack_f* sf_create(uint8_t* buff, size_t buffsize, size_t chunksize, enum sf_RES* res) {
  stack_f* sf;
  if (buffsize < sizeof(stack_f)) {
    *res = sf_SMALLBUFF;
    return NULL;
  }

  sf = (stack_f*)buff;
  sf->buff = buff+sizeof(stack_f);
  sf->chunksize = chunksize;
  sf->buffsize = buffsize-sizeof(stack_f);

  *res = sf_OK;
  return sf;
}

uint8_t* sf_alloc(stack_f* sf) {
  uint8_t* out = sf->buff + sf->allocated;
  sf->allocated += sf->chunksize;
  return out;
}

enum sf_RES sf_free(stack_f* sf) {
  if (sf->allocated == 0) {
    return sf_STACKEMPTY;
  }
  sf->allocated -= sf->chunksize;
  return sf_OK;
}

void sf_freeall(stack_f* sf) {
  sf->allocated = 0;
}

size_t sf_available(stack_f* sf) {
  return sf->buffsize - sf->allocated;
}

size_t sf_used(stack_f* sf) {
  return sf->allocated;
}

bool sf_empty(stack_f* sf) {
  return sf->allocated == 0;
}
