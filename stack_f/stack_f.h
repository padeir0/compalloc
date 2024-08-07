#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum sf_RES {sf_OK, sf_SMALLBUFF, sf_STACKEMPTY};

char* sf_str_res(enum sf_RES res);

typedef struct {
  uint8_t* buff;
  size_t allocated;
  size_t chunksize;
  size_t buffsize;
} stack_f;

stack_f* sf_create(uint8_t* buff, size_t buffsize, size_t chunksize, enum sf_RES* res);

uint8_t* sf_alloc(stack_f* sf);

enum sf_RES sf_free(stack_f* sf);

void sf_freeall(stack_f* sf);

size_t sf_available(stack_f* sf);

size_t sf_used(stack_f* sf);

bool sf_empty(stack_f* sf);
