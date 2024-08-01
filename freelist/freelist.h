#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
} freelist;

freelist* freelist_create(void* buffer, size_t size);

// tries to allocate a object of the given size,
// returns NULL if it fails to allocate
void* freelist_alloc(freelist* fl, size_t size);

// frees an object allocated by the freelist
// if the object was not allocated in that particular freelist,
// or if the object is uncorrectly aligned,
// it returns false
bool freelist_free(freelist* fl, void* obj);

// frees all objects in the free list
void freelist_free_all(freelist* fl);

// returns the amount of memory available
size_t freelist_available(freelist* fl);

// returns the amount of memory used
size_t freelist_used(freelist* fl);

// returns if the heap is empty
bool freelist_empty(freelist* fl);
