#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum pool_RES {
  pool_OK,
  /* indicates the pointer to be freed is not within the pool */
  pool_ERR_BOUNDS,
  /* indicates the pointer to be freed is unaligned with chunk boundaries */
  pool_ERR_ALIGN,
  /* indicates given chunk size is too small (minimum is sizeof(node)) */
  pool_ERR_CHUNK_SIZE, 
  /* indicates given buffer is too small (minimum is sizeof(pool) + chunksize) */
  pool_ERR_SMALL_BUFF,
  /* indicates the given buffer is null */
  pool_ERR_NULL_BUFF
};

char* str_res(enum pool_RES r);

typedef struct _pool_snode {
  struct _pool_snode* next;
} pool_node;

typedef struct {
  pool_node* head;
  pool_node* tail;
  uint8_t* begin;
  uint8_t* end;
  size_t chunksize;
  size_t size;
} pool;

/* returns a pool allocated at the beginning of the buffer
 * if the pool is NULL, then error contains the reason.
 */
pool* pool_create(uint8_t* buff, size_t buff_size, size_t chunksize, enum pool_RES* error);

/* tries to allocate a object of size 'chunksize',
 * returns NULL if it fails to allocate
 */
void* pool_alloc(pool* p);

/* frees an object allocated in the pool,
 * returns error if the pointer is incorrectly aligned
 */
enum pool_RES pool_free(pool* p, void* obj);

/* frees all objects in the pool
 */
void pool_free_all(pool* p);

/* returns the amount of memory available
 */
size_t pool_available(pool* p);

/* returns the amount of memory used
 */
size_t pool_used(pool* p);

/* returns if the pool is empty
 */
bool pool_empty(pool* p);
