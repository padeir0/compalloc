#include "stack_v.h"

char* sv_str_res(enum sv_RES res) {
  switch (res) {
    case sv_OK:
      return "OK";
    case sv_SMALLBUFF:
      return "Provided buffer is too small";
    case sv_STACKEMPTY:
      return "Stack is empty";
  }
  return "???";
}

stack_v* sv_create(uint8_t* buff, size_t buffsize, enum sv_RES* res) {
  stack_v* sv;
  if (buffsize < sizeof(stack_v)) {
    *res = sv_SMALLBUFF;
    return NULL;
  }

  sv = (stack_v*)buff;
  sv->buff = buff + sizeof(stack_v);
  sv->buffsize = buffsize - sizeof(stack_v);
  sv->allocated = 0;

  *res = sv_OK;
  return sv;
}

uint8_t* sv_alloc(stack_v* sv, size_t size) {
  uint8_t* place;
  size_t newsize = size + sizeof(sv_obj_postfix);
  sv_obj_postfix* obj;

  if (sv->buffsize - sv->allocated < newsize) {
    return NULL;
  }

  place = sv->buff + sv->allocated;
  obj = (sv_obj_postfix*)(place + size);
  obj->prev_size = size;
  sv->allocated += newsize;
  return place;
}

enum sv_RES sv_free(stack_v* sv) {
  sv_obj_postfix* obj;
  if (sv->allocated == 0) {
    return sv_STACKEMPTY;
  }
  obj = (sv_obj_postfix*)(sv->buff + sv->allocated - sizeof(sv_obj_postfix));
  sv->allocated -= (obj->prev_size + sizeof(sv_obj_postfix));
  return sv_OK;
}

void sv_freeall(stack_v* sv) {
  sv->allocated = 0;
}

size_t sv_available(stack_v* sv) {
  return sv->buffsize - sv->allocated;
}

size_t sv_used(stack_v* sv) {
  return sv->allocated;
}

bool sv_empty(stack_v* sv) {
  return sv->allocated == 0;
}
