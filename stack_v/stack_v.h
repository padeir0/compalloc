#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum sv_RES {sv_OK, sv_SMALLBUFF, sv_STACKEMPTY};

char* sv_str_res(enum sv_RES res);

typedef struct {
  size_t prev_size;
} sv_obj_postfix;

typedef struct {
  uint8_t* buff;
  size_t allocated;
  size_t buffsize;
} stack_v;

stack_v* sv_create(uint8_t* buff, size_t buffsize, enum sv_RES* res);

uint8_t* sv_alloc(stack_v* sv, size_t size);

enum sv_RES sv_free(stack_v* sv);

void sv_freeall(stack_v* sv);

size_t sv_available(stack_v* sv);

size_t sv_used(stack_v* sv);

bool sv_empty(stack_v* sv);
