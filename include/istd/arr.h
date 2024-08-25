/**
 * \file
 * \brief Simple C dynamic array, written as header-only library.
 *
 * It works somewhat like this:
 *
 * ```
 * -----------------------------------------------------------
 *   | len | availiable | element 1 | element 2 | ... | \0 |
 * -----------------------------------------------------------
 *   ^                             ^
 *   |                    pointer given to you, of type element*
 *   beginning of alloc-ed memory
 * ```
 *
 * \note Uses gcc-s statement expressions, docs for them can be found here: https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 * \note I hate this thing...
 * \todo Macro variable name generators, to 
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
  alignas(alignof(max_align_t)) void* data[];
} _ia_internal_array;

/// \internal
/// Get pointer to internal metadata of array
#define _ia_metadata(array) \
  ((_ia_internal_array*) (((char*) (array)) - sizeof(_ia_internal_array)))

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
#define ia_availiable(array) \
  (_ia_metadata(array)->availiable)

/// \brief Annotate type as array
/// \todo Find a way to make this distinct from normal pointer.
#define ia_arr(type) type*


/// \internal
/// Get address of zero byte after all array items
#define _ia_zero_byte(array)  ({ \
    typeof(array) _ia_arr__zero_byte = (array); \
    ((char*) _ia_arr__zero_byte) + ia_length(_ia_arr__zero_byte) * sizeof(*_ia_arr__zero_byte); \
  })

/// \internal
/// Creates array
/// \param preallocate - Number of elements to reserve space for
/// \param length - Number of elements in the array
/// \param item_size - Size of elements in this new array
void* _ia_make_array(size_t preallocate, size_t length, size_t item_size);

/// \internal
/// Make sure what  this buffer fits at least current number
/// of elements + `num_items`. This **does not** change its length.
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
    size_t _ia_len__array_of = (size); /* Do not compute length twice */ \
    (type*) _ia_make_array(_ia_len__array_of, _ia_len__array_of, sizeof(type)); \
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
#define ia_push(array, item) ({ \
    typeof(array) * _ia_arr__push = &(array); \
    _ia_maybe_grow((void**) _ia_arr__push, sizeof(*array), 1); /* Alloc space if needed */ \
    (*_ia_arr__push)[ia_length(*_ia_arr__push)] = item; /* Store item */ \
    ia_length(*_ia_arr__push)++; \
    *_ia_zero_byte(*_ia_arr__push) = '\0'; /* Update zero byte */ \
  })

/// Pop last item from the array. Does not return it.
/// It also does not `free` or do anything if it is a pointer.
/// That's yout job.
/// \param array - Array to remove last item from
#define ia_pop(array) ({ \
    typeof(array) _ia_arr__pop = (array); \
    --ia_length(_ia_arr__pop); \
    *_ia_zero_byte(_ia_arr__pop) = '\0'; \
  })

/// Free given array.
/// \param array - Array to free
#define ia_destroy(array) free(_ia_metadata(array))

/// Join buffer to array
/// \param array Array to join buffer to
/// \param buffer Buffer to join
/// \param length Length of the buffer
#define ia_joinn(array, buffer, length) \
  _ia_join((void**) &(array), (buffer), (length), sizeof(*array))

/// Join array2 to array
#define ia_join(array, array2) \
  ia_joinn(array, array2, ia_length(array2))

/// Join something zero-terminated to array
#define ia_joinz(array, zterm) { \
    typeof(zterm) _ia_zterm__joinz = (zterm); \
    ia_joinn(array, _ia_zterm__joinz, strlen(_ia_zterm__joinz));\
  }

/// Array containing given string
/// \param str - String to store into that array
#define ia_string_array(ctype, str) ({ \
    /* Typeof(*str)* is required when we pass static string */ \
    typeof(*str)* _ia_str__string_array = (str); \
    size_t _ia_strlen__string_array = strlen(_ia_str__string_array);\
    typeof(*str)* _ia_res__string_array = ia_array_of(_ia_strlen__string_array, typeof(*str));\
    memcpy(_ia_res__string_array, _ia_str__string_array, _ia_strlen__string_array);\
    _ia_res__string_array;\
  })

#endif
