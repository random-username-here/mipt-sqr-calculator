// Just implement it here
#include "istd/arr.h"
#include <string.h>
#include <stdio.h>

void* _ia_make_array(size_t preallocate, size_t length, size_t item_size) {

  if (length > preallocate)
    preallocate = length;

  _ia_internal_array* arr = (_ia_internal_array*)
    calloc(1, sizeof(_ia_internal_array) + preallocate * item_size + 1 /* for zero byte */);

  arr->availiable = preallocate;
  arr->length = length;

  return arr->data;  
}

void _ia_maybe_grow(void** array, size_t item_size, size_t num_items) {

  _ia_internal_array* arr = _ia_metadata(*array);

  if (arr->availiable >= arr->length + num_items)
    return;

  size_t new_size = arr->availiable+2;
  while (new_size < arr->length + num_items)
    new_size = new_size * 3 / 2;

  void* new_array = _ia_make_array(new_size, arr->length, item_size);
  memcpy(new_array, arr->data, arr->length * item_size);

  ia_destroy(*array);
  *array = new_array;
}

void _ia_join(void** array,  const void* source,
              size_t num_items, size_t item_size) {
  _ia_maybe_grow(array, item_size, num_items);
  memcpy(
    (char*) *array + item_size * ia_length(*array),
    source,
    item_size * num_items
  );
  ia_length(*array) += num_items;
  *(((char*) *array) + item_size * ia_length(*array)) = '\0';
}
