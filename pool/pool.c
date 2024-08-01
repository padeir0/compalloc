// this is a hand translation of code previously written in millipascal
#include "pool.h"
#include <strings.h>

int32_t distance(void* a, void* b) {
  if (a > b) {
    return a-b;
  } else {
    return b-a;
  }
}

void set_list(pool* p) {
  node* curr = (node*)p->begin;
  // we need this because of alignment, the chunks may not align
  // and leave a padding at the end of the buffer
  node* end = (node*)p->end - p->chunksize;

  p->head = curr;
  while (curr < end) {
    curr->next = curr + p->chunksize;
    curr = curr->next;
  }

  // curr is at the edge of the buffer, and may not be valid
  // in case the end is not aligned, we leave padding
  if ((void*)curr + p->chunksize != p->end) {
    p->end = (void*)curr;
    curr -= p->chunksize;
    p->size = distance(p->begin, p->end);
  }

  curr->next = NULL;
  p->tail = curr;
}

const size_t min_chunk_size = sizeof(node);

pool* pool_create(void* buff, size_t buffsize, size_t chunksize, enum RES* out) {
  pool* p;
  bool ok;

  if (chunksize < min_chunk_size) {
    *out = ERR_CHUNK_SIZE;
    return NULL;
  }

  if (buffsize < sizeof(pool) + chunksize) {
    *out = ERR_SMALL_BUFF;
    return NULL;
  }

  p = (pool*)buff;
  p->begin = buff + sizeof(pool);
  p->end = buff + buffsize;
  p->chunksize = chunksize;
  p->size = distance(p->begin, p->end);

  bzero(p->begin, p->size);
  set_list(p);
  return p;
}

void* pool_alloc(pool* p) {
  void* curr;
  if (p->head == NULL) {
    return NULL;
  }

  curr = p->head;
  p->head = p->head->next;

  if (p->head == NULL) {
    p->tail = NULL;
  }
  return curr;
}

enum RES pool_free(pool* p, void* ptr) {
  int32_t position;
  node* new;

  if (!(p->begin <= ptr && ptr < p->end)) {
    return ERR_BOUNDS;
  }

  if (distance(ptr, p->begin) % p->chunksize != 0) {
    return ERR_ALIGN;
  }

  new = (node*)ptr;
  new->next = NULL;

  if (p->head == NULL) {
    p->head = new;
    p->tail = new;
    return OK;
  }

  p->tail->next = new;
  p->tail = new;
  return OK;
}

size_t pool_used(pool* p) {
  return p->size - pool_available(p);
}

size_t pool_available(pool* p) {
  size_t total = 0;
  node* curr;
  while (curr != NULL) {
    total += p->chunksize;
    curr = curr->next;
  }
  return total;
}

bool pool_empty(pool* p) {
  size_t total = pool_available(p);
  if (total < p->size) {
    return false;
  }

  if (p->size < total) {
    printf("seeing this error means there's something is wrong in this library");
    abort();
  }
  
  return true;
}
