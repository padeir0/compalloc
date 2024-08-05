#include "pool.h"
#include <strings.h> /*TODO: reimplement bzero and remove this dependency*/

#define offsetnode(a, x) (pool_node*)((uint8_t*)a + x)

size_t distance(uint8_t* a, uint8_t* b) {
  if (a > b) {
    return a-b;
  } else {
    return b-a;
  }
}

char* str_res(enum pool_RES r) {
  switch (r) {
    case pool_OK:
      return "OK";
    case pool_ERR_BOUNDS:
      return "Pointer is out of bounds";
    case pool_ERR_ALIGN:
      return "Pointer is out of alignment";
    case pool_ERR_CHUNK_SIZE:
      return "Provided chunk size is too small";
    case pool_ERR_SMALL_BUFF:
      return "Provided buffer is too small";
    case pool_ERR_NULL_BUFF:
      return "Buffer is NULL";
  }
  return "??";
}

void set_list(pool* p) {
  pool_node* curr = (pool_node*)p->begin;
  /* we need this because of alignment, the chunks may not align
   * and leave a padding at the end of the buffer
   */
  pool_node* end = offsetnode(p->end, -p->chunksize);

  p->head = curr;
  while (curr < end) {
    curr->next = offsetnode(curr, p->chunksize);
    curr = curr->next;
  }

  /* curr is at the edge of the buffer, and may not be valid
   * in case the end is not aligned, we leave padding
   */
  if ((uint8_t*)curr + p->chunksize != p->end) {
    p->end = (uint8_t*)curr;
    curr = offsetnode(curr, -p->chunksize);
    p->size = distance(p->begin, p->end);
  }

  curr->next = NULL;
  p->tail = curr;
}

const size_t min_chunk_size = sizeof(pool_node);

pool* pool_create(uint8_t* buff, size_t buffsize, size_t chunksize, enum pool_RES* out) {
  pool* p;

  if (buff == NULL) {
    *out = pool_ERR_NULL_BUFF;
    return NULL;
  }

  if (chunksize < min_chunk_size) {
    *out = pool_ERR_CHUNK_SIZE;
    return NULL;
  }

  if (buffsize < sizeof(pool) + chunksize) {
    *out = pool_ERR_SMALL_BUFF;
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

enum pool_RES pool_free(pool* p, void* ptr) {
  pool_node* new;

  if (!(p->begin <= (uint8_t*)ptr && (uint8_t*)ptr < p->end)) {
    return pool_ERR_BOUNDS;
  }

  if (distance(ptr, p->begin) % p->chunksize != 0) {
    return pool_ERR_ALIGN;
  }

  new = (pool_node*)ptr;
  new->next = NULL;

  if (p->head == NULL) {
    p->head = new;
    p->tail = new;
    return pool_OK;
  }

  p->tail->next = new;
  p->tail = new;
  return pool_OK;
}

void pool_free_all(pool* p) {
  set_list(p);
}

size_t pool_available(pool* p) {
  size_t total = 0;
  pool_node* curr = p->head;
  while (curr != NULL) {
    total += p->chunksize;
    curr = curr->next;
  }
  return total;
}

size_t pool_used(pool* p) {
  return p->size - pool_available(p);
}

bool pool_empty(pool* p) {
  size_t total = pool_available(p);
  if (total < p->size) {
    return false;
  }
  return true;
}
