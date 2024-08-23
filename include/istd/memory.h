/**
 * \file
 * \brief A toy C memory allocator, doing things of `malloc`/`calloc`/`free`
 */

#include <stddef.h>

#ifndef ISTD_MEMEORY
#define ISTD_MEMORY

/// \brief Allocate `size` or more bytes of memory
/// If it failed to do so, it returns `NULL`
///
/// \param size How much memory you ask for
/// \returns    Pointer to your new memory
void* im_alloc(size_t size);

/// \brief Free given memory buffer
/// 
/// That buffer must be allocated by `im_alloc`.
/// 
/// \param addr Memory you don't need anymore
void im_free(void* addr);

/// \brief Grow given memory to `size` bytes.
/// 
/// If there is not enough memory to grow after
/// this block, this will move contents of given buffer
/// to new location where there is space,
/// and return the new pointer.
///
/// If this fails to do what user wants, it will return
/// `NULL` and leave original buffer intact.
///
/// \param addr Address of buffer
/// \param size How much bytes you need to be
///             in that buffer.
/// \returns    Given buffer or new one, if given
///             buffer could not be grown.
void* im_realloc(void* addr, size_t size);

/// \brief Get number of bytes availiable in given buffer.
/// 
/// After `im_alloc` or `im_realloc` you can get buffer
/// which is larger than you expected. This gets the 
/// actual size of the buffer you got.
///
/// \param addr Address you want to get info about
/// \returns    Numer of bytes you can use after that address.
size_t im_avail(void* addr);

/// \brief Get information about current state of memory segments
/// \param callback Callback to call for each segment.
///                 First param is buffer address, second is size,
///                 third is `1` if it is used or `0` if not.
void im_get_info(void (*callback)(void*, size_t, int));

#endif
