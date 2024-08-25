/**
 * \file
 * \brief Simple C dynamic array, written as header-only library.
 *
 * It works somewhat like this:
 *
 * ```
 *     _ia_memory_begin_offset (for alignment)   
 *   |<---------------------->|
 * -----------------------------------------------------------
 *   |     | len | availiable | element 1 | element 2 | ... | \0 |
 * -----------------------------------------------------------
 *   ^                             ^
 *   |                    pointer given to you, of type element*
 *   beginning of alloc-ed memory
 * ```
 *
 * This library is header-only, so in one C file you should put
 * `#define ISTD_ARR_IMPLEMENTATION` to implement its functions.
 *
 * \note Uses gcc-s statement expressions, docs for them can be found here: https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 */

#ifndef ISTD_ARR
#define ISTD_ARR

#include <stdlib.h>
#include <stdalign.h>
#include <stddef.h>

/// \internal
/// Structure stored at the beginning of the array,
/// containing all data used internally by array functions.
typedef struct {
  size_t length;
  size_t availiable;
} _ia_meta_t;

/// \internal
/// Get pointer to internal metadata of array
#define _ia_metadata(array) \
  ((_iarr_meta_t*) (((char*) (array)) - sizeof(_iarr_meta_t)))

/// Get length of dynamic array
/// \param array - The array
/// \returns Array length in items (not bytes).
#define ia_length(array) \
  (_ia_metadata(array)->length)

/// Get number of elements array has memory for
/// Must be >= `ia_length(array)`, nothing else is
/// guaranteed.
/// \param array - Array
/// \returns Number of element for which there is enough space
#define ia_real_length(array)

/// \internal
/// Distance from array element location to beginning
/// of allocated memory.
/// Calculated as `ceil(sizeof(metadata) / max_align) * max_align`
#define _ia_memory_begin_offset \
  ((sizeof(_ia_meta_t) + alignof(max_align_t) - 1) \
   / alignof(max_align_t) * alignof(max_align_t))

/// \internal
/// Get address of zero byte after all array items
#define _ia_zero_byte(array)  { \
    typeof(array) _ia_arr = (array); \
    ((char*) _ia_arr) + ia_length(_ia_arr) * sizeof(*_ia_arr); \
  }

/// \internal
/// Creates array
/// \param preallocate - Number of elements to reserve space for
/// \param length - Number of elements in the array
/// \param item_size - Size of elements in this new array
void* _ia_make_array(size_t preallocate, size_t length, size_t item_size);

/// \internal
/// Make sure what  this buffer fits at least current number
/// of elements + `num_items`.
/// \param array - Array for which to grow buffer if needed
/// \param item_size - Size of items in said array
/// \param num_items - Number of items, for which we need space
void _ia_maybe_grow(void** array, size_t item_size, size_t num_items);

/// \internal
/// Join given buffer to the end of the array
/// \param array - Array to join items to
/// \param source - Source buffer, containing items to join
/// \param num_items - Number of items to add into array 
/// \param item_size - Size of one item in buffer, in bytes.
/// \todo Add restrict?
void _ia_join(void** array,  const void* source,
              size_t num_items, size_t item_size);

/// Create new array for items of given type
/// \param type - Type of items, like `int`
/// \returns - Array for items, of type `type*`
#define ia_empty_array(type)   ((type*) _ia_make_array(0, 0, sizeof(type)))

/// Create array with N zeroed items
/// \param type - Type of items inside array, like `int`
/// \param size - Number of items. Each item is zeroed.
/// \returns - Array of type `type*`
#define ia_array_of(size, type) ({ /* This uses statement expressions */ \
    size_t _ia_len = (size); /* Do not compute length twice */ \
    (type*) _ia_make_array(_ia_len, _ia_len, sizeof(type)); \
    /* ^ Result of that line is returned */ \
  })

/// Create array with space preallocated for N items.
/// Usefull if you know how many items will be push-ed into array.
/// \param type - Type of items in array
/// \param size - Number of items for which there is space inside array
/// \returns - Empty array into which you can insert `SIZE`
///            items without reallocation
#define ia_array_for(size, type) \
  ((type*) _ia_make_array((size), _ia_len, sizeof(type)))

/// Append item into given array
/// \param array - Array to push item into
/// \param item - Item to push.
#define ia_push(array, item) { \
    typeof(array) * _ia_arr = &(array); \
    _ia_maybe_grow((void**) _ia_arr, sizeof(*array), 1); /* Alloc space if needed */ \
    (*_ia_arr)[ia_length(*_ia_arr)++] = item; /* Store item */ \
    *_ia_zero_byte(*_ia_arr) = '\0'; /* Update zero byte */ \
  }

/// Pop last item from the array. Does not return it.
/// It also does not `free` or do anything if it is a pointer.
/// That's yout job.
/// \param array - Array to remove last item from
#define ia_pop(array) { \
    typeof(array) _ia_arr = (array); \
    --ia_length(_ia_arr); \
    *_ia_zero_byte(_ia_arr) = '\0' \
  }

/// Free given array.
/// \param array - Array to free
#define ia_destroy(array) { free((char*) (array) - _ia_memory_begin_offset); }

//--- Implementation -------------------------------------//

//#ifdef ISTD_ARR_IMPLEMENTATION

void* _ia_make_array(size_t preallocate, size_t length, size_t item_size) {
  
}

void _ia_maybe_grow(void** array, size_t item_size, size_t num_items) {

}

void _ia_join(void** array,  const void* source,
              size_t num_items, size_t item_size) {
  
}

//#endif

#endif
