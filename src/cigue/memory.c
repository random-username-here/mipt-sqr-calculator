#include "cigue/memory.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

cigue_mem_buffer* cigue_mem_new_buffer(size_t initial_size) {
  cigue_mem_buffer* new = malloc(sizeof(cigue_mem_buffer));
  new->memory = malloc(initial_size);
  new->avail_size = initial_size;
  new->alloc_point = 0;
  return new;
}

void cigue_mem_free_buffer(cigue_mem_buffer* buf) {
  free(buf->memory);
  free(buf);
}

void cigue_mem_new_frame(cigue_mem_buffer* buf) {
  buf->alloc_point = 0;
}

void* cigue_mem_alloc(cigue_mem_buffer* buf, size_t size) {
  while (buf->alloc_point + size > buf->avail_size)
    // Нужно больше места!
    buf->memory = realloc(buf->memory, buf->avail_size * 3 / 2 + 1);
  void* handle = buf->memory + buf->alloc_point;
  buf->alloc_point += size;
  return handle;
}

void* cigue_mem_save(cigue_mem_buffer* buf, const void* object, size_t size) {
  void* ptr = cigue_mem_alloc(buf, size);
  memcpy(ptr, object, size);
  return ptr;
}

char* cigue_mem_save_str(cigue_mem_buffer* buf, const char* str) {
  return cigue_mem_save(buf, str, strlen(str) + 1);
}
