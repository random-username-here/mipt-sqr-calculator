#include "cigue/memory.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

cigue_mem_buffer* cigue_mem_new_buffer(size_t initial_size) {

  cigue_mem_buffer* new_buf = (cigue_mem_buffer*) calloc(1, sizeof(cigue_mem_buffer));
  assert(new_buf != NULL && "Buffer for UI was not created. Probbably you do not have enough memory");
  
  new_buf->memory = calloc(1, initial_size);
  new_buf->avail_size = initial_size;
  new_buf->alloc_point = 0;
  
  return new_buf;
}

void cigue_mem_free_buffer(cigue_mem_buffer* buf) {

  assert(buf && "Buffer to free must be != NULL");
  
  free(buf->memory); // = nullptr;
  free(buf);
}

void cigue_mem_new_frame(cigue_mem_buffer* buf) {

  assert(buf && "Buffer to begin new frame must be != NULL");
  
  buf->alloc_point = 0;
}

void* cigue_mem_alloc(cigue_mem_buffer* buf, size_t size) {

  assert(buf && "Buffer to alloc data in must be != NULL");
  // При реаллокации буффер перезжает вместе с указателями
  // Нужны "умные указатели", не ломающиеся после переезда.
  // Или какая-то ещё муть.
  // FIXME FIXME FIXME!
  assert(buf->alloc_point + size < buf->avail_size && "Buffer must have enough memory!"); // TODO: null?
  /*while (buf->alloc_point + size > buf->avail_size) {
    // Нужно больше места!
    size_t new_sz = buf->avail_size * 3 / 2 + 1;
    buf->memory = realloc(buf->memory, new_sz);
    buf->avail_size = new_sz;
  }*/
  void* handle = (void*)((char*)buf->memory + buf->alloc_point);
  memset(handle, 0, size);
  buf->alloc_point += size;
  
  return handle;
}

void* cigue_mem_alloc_aligned(cigue_mem_buffer* buf, size_t size, size_t align) {

  assert(buf && "Buffer to alloc data in must be != NULL");
  
  buf->alloc_point += (align - (uintptr_t) ((char*) buf->memory + buf->alloc_point) % align) % align;
  void* ptr = cigue_mem_alloc(buf, size);
  
  return ptr;
}

void* cigue_mem_save(cigue_mem_buffer* buf, const void* object, size_t size) {

  assert(buf && "Buffer to save object into must be != NULL");
  assert(object && "Object to save into buffer must be != NULL");
  
  void* ptr = cigue_mem_alloc(buf, size);
  memcpy(ptr, object, size);
  
  return ptr;
}

char* cigue_mem_save_str(cigue_mem_buffer* buf, const char* str) {

  assert(buf && "Buffer to save string into must be != NULL");
  assert(str && "String to save into buffer must be != NULL");
  
  return (char*) cigue_mem_save(buf, str, strlen(str) + 1);
}

void cigue_assert_buf_memory(cigue_mem_buffer* buf, const void* mem) {

  const char* begin = (char*) buf->memory,
            *end = begin + buf->avail_size;
  
  assert((const char*) mem < end && (const char*) mem >= begin && "Pointer must be from gui buffer.");
}
