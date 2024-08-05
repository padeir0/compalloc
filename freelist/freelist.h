#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum fl_RES {
  fl_OK,
  /* Buffer is too small */
  fl_ERR_SMALLBUFF,
  /* Pointer to be freed is out of bounds */
  fl_ERR_BOUNDS
};

char* fl_str_res(enum fl_RES res);

typedef struct {
  size_t size;
} obj_header;

typedef struct _fl_node {
  size_t size;
  struct _fl_node *next;
} fl_node;

typedef struct {
  fl_node* head;
  uint8_t* begin;
  uint8_t* end;
  size_t   size;
} freelist;

size_t fl_pad(size_t size);
size_t fl_objsize(void* size);

freelist* fl_create(uint8_t* buffer, size_t size, enum fl_RES* res);

/* tries to allocate a object of the given size,
 * returns NULL if it fails to allocate
 */
void* fl_alloc(freelist* fl, size_t size);

/* frees an object allocated by the freelist
 * if the object was not allocated in that particular freelist,
 * or if the object is uncorrectly aligned,
 * it returns false
 */
enum fl_RES fl_free(freelist* fl, void* obj);

/* frees all objects in the free list */
void fl_free_all(freelist* fl);

/* returns the amount of memory available */
size_t fl_available(freelist* fl);

/* returns the amount of memory used */
size_t fl_used(freelist* fl);

/* returns if the heap is empty */
bool fl_empty(freelist* fl);
