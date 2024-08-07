#include "arena.h"

char* arena_str_res(enum arena_RES res) {
  switch(res){
    case arena_OK:
      return "OK";
    case arena_NULL_BUFF:
      return "Provided buffer is null";
    case arena_TOO_SMALL:
      return "Provided buffer is too small";
  }
  return "???";
}

size_t distance(uint8_t* a, uint8_t* b) {
  if (a > b) {
    return a-b;
  } else {
    return b-a;
  }
}

arena* arena_create(uint8_t* buffer, size_t size, enum arena_RES* res) {
  arena* out;
  if (buffer == NULL) {
    *res = arena_NULL_BUFF;
    return NULL;
  }

  if (size < sizeof(arena)) {
    *res = arena_TOO_SMALL;
    return NULL;
  }

  out = (arena*)buffer;
  out->buffer = buffer + sizeof(arena);
  out->buffsize = size - sizeof(arena);
  out->allocated = 0;

  return out;
}

void* arena_alloc(arena* a, size_t size) {
  void* out = (void*)(a->buffer + a->allocated);
  if (a->allocated+size >= a->buffsize) {
    return NULL;
  }
  a->allocated += size;
  return out;
}

void arena_free_all(arena* a) {
  a->allocated = 0;
}

size_t arena_available(arena* a) {
  return a->buffsize - a->allocated;
}

size_t arena_used(arena* a) {
  return a->allocated;
}

bool arena_empty(arena* a) {
  return a->allocated == 0;
}
