#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

enum RES {OK, ERR_BOUNDS, ERR_ALIGN, ERR_CHUNK_SIZE, ERR_SMALL_BUFF};

typedef struct _snode {
  struct _snode* next;
} node;

typedef struct {
  node* head;
  node* tail;
  void* begin;
  void* end;
  int32_t chunksize;
  int32_t size;
} pool;

// returns a pool allocated at the beginning of the buffer
pool* pool_create(void* buff, size_t buff_size, size_t chunksize, enum RES* out);

// tries to allocate a object of size 'chunksize',
// returns NULL if it fails to allocate
void* pool_alloc(pool* p);

// frees an object allocated by the pool,
// if the object was not allocated in that particular pool,
// or if the object is uncorrectly aligned,
// it returns false
enum RES pool_free(pool* p, void* obj);

// frees all objects in the pool
void pool_free_all(pool* p);

// returns the amount of memory available
size_t pool_available(pool* p);

// returns the amount of memory used
size_t pool_used(pool* p);

// returns if the pool is empty
bool pool_empty(pool* p);
