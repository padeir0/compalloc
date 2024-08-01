#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
} arena;

// returns a arena allocated at the beginning of the buffer
arena* arena_create(void* buffer, size_t size);

// returns NULL if it fails to allocate
void* arena_alloc(arena* a, size_t size);

// frees the entire arena
void arena_free_all(arena* a);

// returns the amount of memory available
size_t arena_available(arena* a);

// returns the amount of memory used
size_t arena_used(arena* a);

// returns true if the arena is empty
bool arena_empty(arena* a);
