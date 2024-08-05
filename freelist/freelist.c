#include "freelist.h"
#include <stdio.h>

#define WORD sizeof(void*)
#define offsetnode(a, x) (fl_node*)((uint8_t*)a + x)

size_t distance(uint8_t* a, uint8_t* b) {
  if (a > b) {
    return a-b;
  } else {
    return b-a;
  }
}

size_t fl_pad(size_t size) {
  size = size + sizeof(obj_header);
  if (size%WORD != 0) {
    return size + (WORD-size%WORD);
  }
  /* objects need space for a Node when deallocated */
  if (size < sizeof(fl_node)) {
    size = sizeof(fl_node);
  }
  return size;
}

char* fl_str_res(enum fl_RES res) {
  switch (res) {
    case fl_OK:
      return "OK";
    case fl_ERR_SMALLBUFF:
      return "Provided buffer is too small";
    case fl_ERR_BOUNDS:
      return "Pointer is out of bounds";
  }
  return "??";
}

freelist* fl_create(uint8_t* buffer, size_t size, enum fl_RES *res) {
  freelist* fl;
  if (size < sizeof(freelist) + sizeof(fl_node)) {
    *res = fl_ERR_SMALLBUFF;
    return NULL;
  }

  fl = (freelist*)buffer;
  fl->head = (fl_node*)(buffer + sizeof(freelist));
  fl->head->size = size - sizeof(freelist);
  fl->head->next = NULL;

  fl->begin = (uint8_t*)fl->head;
  fl->end = buffer+size;

  fl->size = distance(fl->begin, fl->end);
  return fl;
}

uint8_t* pop(freelist* fl, fl_node* prev, fl_node* curr) {
  if (prev != NULL) {
    prev->next = curr->next;
  } else {
    fl->head = curr->next;
  }
  return (uint8_t*)curr;
}

uint8_t* split(freelist* fl, fl_node* prev, fl_node* curr, size_t requested_size) {
  fl_node* newnode;

  newnode = offsetnode(curr, requested_size);
  newnode->size = curr->size - requested_size;
  newnode->next = curr->next;

  curr->size = requested_size;
  curr->next = newnode;

  return pop(fl, prev, curr);
}

void getnode(freelist* fl, size_t size, uint8_t** outptr, size_t* allocsize) {
  fl_node* curr;
  fl_node* prev;

  if (fl->head == NULL) {
    *outptr = NULL;
    *allocsize = 0;
    return;
  }

  curr = fl->head;
  prev = NULL;

  while (curr != NULL) {
    if (curr->size == size) {
      *outptr = pop(fl, prev, curr);
      *allocsize = size;
      return;
    }

    if (curr->size > size) {
      /* if we allocate an object and the remaining
       * size is not sufficient for a node,
       * we allocate the full space, without splitting
       */
      if (curr->size - size < sizeof(fl_node)) {
        *outptr = pop(fl, prev, curr);
        *allocsize = curr->size;
        return;
      }
      *outptr = split(fl, prev, curr, size);
      *allocsize = size;
      return;
    }

    prev = curr;
    curr = curr->next;
  }

  *outptr = NULL;
  *allocsize = 0;
  return;
}

void* fl_alloc(freelist* fl, size_t size) {
  uint8_t* p;
  size_t allocsize;

  size = fl_pad(size);

  getnode(fl, size, &p, &allocsize);
  if (p == NULL) {
    return NULL;
  }
  ((obj_header*) p)->size = allocsize;
  p += sizeof(obj_header);
  return p;
}

void append(fl_node* prev, fl_node* new) {
  if (offsetnode(prev, prev->size) == new) {
    /* coalescing: append */
    prev->size = prev->size + new->size;
    return;
  }
  prev->next = new;
  new->next = NULL;
  return;
}

void prepend(freelist* fl, fl_node* new) {
  if (offsetnode(new, new->size) == fl->head) {
    /* coalescing: prepend */
    new->size = new->size + fl->head->size;
    new->next = fl->head->next;
    fl->head = new;
    return;
  }

  new->next = fl->head;
  fl->head = new;
  return;
}

void join(fl_node* prev, fl_node* new, fl_node* curr) {
  size_t size;

  if (offsetnode(prev, prev->size) == new) {
    /* coalescing: append */
    size = prev->size + new->size;

    if (offsetnode(prev, size) == curr) {
			/* in this case, prev, new and curr are adjacent */
			prev->size = size + curr->size;
			prev->next = curr->next;
			return;
    }
    /* here only prev and new are adjacent */
    prev->size = size;
    return;
  }

  if (offsetnode(new, new->size) == curr) {
    /* coalescing: prepend */
    prev->next = new;
    new->size = new->size + curr->size;
    new->next = curr->next;
    return;
  }

  prev->next = new;
  new->next = curr;
  return;
}

size_t fl_objsize(void* ptr) {
  obj_header* obj = (obj_header*)((uint8_t*)ptr - sizeof(obj_header));
  return obj->size;
}

enum fl_RES fl_free(freelist* fl, void* p) {
  size_t size;
  fl_node* new; fl_node* prev; fl_node* curr;
  uint8_t* obj = (uint8_t*)p;

  if (obj < fl->begin || fl->end < obj) {
    return fl_ERR_BOUNDS;
  }

  size = fl_objsize(obj);
  new = (fl_node*)(obj-sizeof(obj_header));

  new->size = size;
  new->next = NULL;

  if (fl->head == NULL) {
    fl->head = new;
    return fl_OK;
  }

  if (new < fl->head) {
    prepend(fl, new);
    return fl_OK;
  }

  prev = NULL;
  curr = fl->head;

  while (curr != NULL) {
    if (prev != NULL) {
      if (prev < new && new < curr) {
        /* in this case, 'new' is a middle node */
        join(prev, new, curr);
        return fl_OK;
      }
    }
    
    prev = curr;
    curr = curr->next;
  }

  /* in this case, 'new' is the last node */
  append(prev, new);
  return fl_OK;
}

void fl_free_all(freelist* fl) {
  fl->head = (fl_node*)fl->begin;
  fl->head->size = fl->size;
  fl->head->next = NULL;
}

size_t fl_available(freelist* fl) {
  fl_node* curr = fl->head;
  size_t total = 0;

  while (curr != NULL) {
    total += curr->size;
    curr = curr->next;
  }
  return total;
}

size_t fl_used(freelist* fl) {
  return fl->size - fl_available(fl);
}

bool fl_empty(freelist* fl) {
  return fl_available(fl) == fl->size;
}

