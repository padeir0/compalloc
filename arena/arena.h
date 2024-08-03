#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum arena_RES {
  arena_OK,
  arena_NULL_BUFF,
  arena_TOO_SMALL
};

char* arena_str_res(enum arena_RES res);

typedef struct {
  uint8_t* buffer;
  size_t   buffsize;
  uint8_t* head;
} arena;

/* returns a arena allocated at the beginning of the buffer */
arena* arena_create(uint8_t* buffer, size_t size, enum arena_RES* res);

/* returns NULL if it fails to allocate */
void* arena_alloc(arena* a, size_t size);

/* frees the entire arena */
void arena_free_all(arena* a);

/* returns the amount of memory available */
size_t arena_available(arena* a);

/* returns the amount of memory used */
size_t arena_used(arena* a);

/* returns true if the arena is empty */
bool arena_empty(arena* a);
