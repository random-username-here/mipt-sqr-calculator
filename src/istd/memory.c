#include <string.h>
#include <sys/mman.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h> // TODO: drop this out of here
#include <stdbool.h>
#include <stdalign.h>
#include <assert.h>
#include "istd/memory.h"

//---- Defines -------------------------------------------//

/// Align to align allocated buffers to
#define MAX_ALIGN alignof(max_align_t)

///           MIN_SIZE_TO_SPLIT
///            ->|-------|<-
/// -----------------------------------
///  | malloc-ed | ..... | used memory
/// -----------------------------------
///
/// Max size of segment we will join to part of memory we
/// are giving away. If it is larger, it will be left alone
/// as new separate segment.
///
#define MIN_SIZE_TO_SPLIT (sizeof(im_segment) + MAX_ALIGN * 4)

//---- Misc. helpfull stuff ------------------------------//

/// Round size up to max alignment.
static void round_size(size_t *size) {
  assert(size);
  *size = (*size + MAX_ALIGN - 1) / MAX_ALIGN * MAX_ALIGN;
}

//---- Segments ------------------------------------------//

/// Memory segment - part of the memory, which can be used
/// by one `im_alloc`. Or it can be free.
typedef struct im_segment {
  
  /// Previous segment
  struct im_segment* prev;

  /// Is this segment used by someone,
  /// or can we take it?
  bool used;

  /// Number of bytes **in the buffer**
  size_t length;

  /// Buffer which can be given to user.
  /// Has `length` bytes in it.
  alignas(alignof(max_align_t)) char buffer[];

} im_segment;

/// All memory is given in range `[begin, end)`
static im_segment *begin, *end;

/// Gets next segment after this one
/// If there is no segment, returns `NULL`
static im_segment* next_segment(im_segment* seg) {
  im_segment* next = (im_segment*) ((char*) seg + sizeof(im_segment) + seg->length);
  if (next >= end)
    return NULL;
  return next;
}

/// Gets segment corresponding to given ptr
static im_segment* segment_of_ptr(void* ptr) {
  return (im_segment*) ((char*) ptr - sizeof(im_segment));
}

/// Merge given segment with next segment.
///
/// -----------------------------     ---------------------------
///  | seg | joined | new_next |  -->  | seg....... | new_next |
/// -----------------------------     ---------------------------
///
static void merge_segment_with_next(im_segment* seg) {
  im_segment* joined = next_segment(seg);
  assert(joined); // We need something to join to

  im_segment* new_next = next_segment(joined);
  if (new_next) // In case we join two last segments, there is no `new_next`
    new_next->prev = seg;

  seg->length += joined->length + sizeof(im_segment);
}

/// We got segment `seg`, we want to use it to store buffer
/// of size `asked_size`.
///
/// But sometimes using the whole segment for `asked_size` is
/// nor great (imagine storing each 16 byte value in 4096 byte buffers).
/// So sometimes we should split given buffer into two - one of
/// approporiate size to be used, other will be left availiable
/// for some other call of `im_alloc()` to take.
///
/// Sometimes it does nothing, because sometimes it is better not
/// to split that segment.
///
///                                 ->|-----|<- asked_size
/// ----------------------------     -------------------------------
///  | seg        | after_new |  -->  | seg | new_seg | after_new |
/// ---------------------------      -------------------------------
///
static void maybe_split_segment(im_segment* seg, size_t asked_size) {
  round_size(&asked_size);
  if (seg->length - asked_size < MIN_SIZE_TO_SPLIT) {
    return; // In this case we should not split that
  }
  
  // Split this segment
  im_segment* after_new = next_segment(seg); // NULL if we split last
  
  // Setup new segment
  im_segment* new_seg = (im_segment*) (seg->buffer + asked_size);
  new_seg->prev = seg;
  new_seg->length = seg->length - asked_size - sizeof(im_segment);
  new_seg->used = false;

  seg->length = asked_size;

  // Add link to newly made segment 
  if (after_new)
    after_new->prev = new_seg;
}

//---- Mmap part -----------------------------------------//

static void try_init_memory() {
  size_t page = (size_t) getpagesize();
  begin = mmap(
      NULL, page,
      PROT_READ|PROT_WRITE,
      MAP_PRIVATE|MAP_ANON,
      -1, 0
  );
  if (begin == MAP_FAILED) {
    begin = end = MAP_FAILED;
    return;
  }
  end = (im_segment*) ((char*) begin + page);

  // Setup first segment
  begin->used = false;
  begin->length = page - sizeof(im_segment);
  begin->prev = NULL;
}

__attribute__((constructor)) static void _init_im_memory() {
  try_init_memory();
}

/// Add another page to the memory
/// Returns true if mmap did not fail
static bool grow_memory() {
  size_t page = (size_t) getpagesize();
  void* res = mmap(
      end, page,
      PROT_READ|PROT_WRITE,
      MAP_FIXED|MAP_PRIVATE|MAP_ANON,
      -1, 0
  );
  assert(res == end);
  if (res == MAP_FAILED)
    return false;
  *((char**) &end) += page;
  return true;
}

//----- Implementations ----------------------------------//

/// Chose which segment is better to use
static im_segment* chose_segment(im_segment* prev, im_segment* current, size_t size) {
  return prev;
}

void* im_alloc(size_t size) {
  // TODO: check for giant sizes.
  // TODO: use other page sizes for giant allocations

  if (begin == MAP_FAILED) {
    // Base buffer faled to allocate some time ago
    try_init_memory();
    if (begin == MAP_FAILED)
      // And it failed to init now...
      return NULL;
  }

  round_size(&size);
  im_segment *current = begin, *last_element = NULL;
  im_segment *best = NULL;

  while (current) {
    if (!current->used) {
      // Free segment
      if (current->length >= size) {
        // And it has enough space
        if (!best)
          best = current; // First segment we found, it is the best
        else
          best = chose_segment(best, current, size); // Chose between two
      }
    } else {
      // This one is used
      // ignore it
    }
    last_element = current;
    current = next_segment(current);
  }

  if (best) {
    // We found at least one segment,
    // no need to do anything
  } else {
    // No space in memory, we should grow it...
    while (last_element->used || last_element->length < size) {
      im_segment* new_mem_begin = end;
      if (!grow_memory()) {
        // We failed to grow memory
        return NULL;
      }
      if (!last_element->used) {
        // Extend unused segment at the end of buffer
        last_element->length += (size_t) getpagesize();
      } else {
        // We don't have unused segment at the end, extend buffer.
        new_mem_begin->length = (size_t) getpagesize() - sizeof(im_segment);
        new_mem_begin->prev = last_element;
        new_mem_begin->used = false;
        last_element = new_mem_begin;
      }
    }
    best = last_element;
  }
 
  assert(!best->used);
  assert(best->length >= size);

  maybe_split_segment(best, size);
  best->used = true;

  // Zero that buffer
  // NOTE: maybe do like actual malloc and fill it with
  //       specific values, like 0xbada110c
  memset(best->buffer, 0, best->length);
  
  return best->buffer;
}


void* im_realloc(void* ptr, size_t size) {
  round_size(&size);
  im_segment* current = segment_of_ptr(ptr);
  if (size <= current->length) {
    // Realloc tries to allocate smaller array, do nothing
    return ptr;
  }
  // We need to grow
  im_segment* next = next_segment(current);
  // There can be only one free segment after this
  if (next && !next->used) {
    // ... And it is. Maybe we can expand into it
    size_t avail_length = current->length + sizeof(im_segment) + next->length;
    if (avail_length >= size) {
      next = NULL; // Next will be invalid after merge
      merge_segment_with_next(current);
      maybe_split_segment(current, size);
      return current->buffer;

    } 
  }
  // Here we did not fit in...
  // Move into other buffer
  void* new_buf = im_alloc(size);
  if (!new_buf) // We failed...
                //
    return NULL;
  memcpy(new_buf, ptr, current->length);
  im_free(ptr);
  return new_buf;
}

void im_free(void *addr) {
  im_segment* seg = segment_of_ptr(addr);
  
  // Ignore double free
  if (!seg->used) 
    return;

  seg->used = false;

  im_segment* next = next_segment(seg);
  if (next && !next->used) {
    // 
    merge_segment_with_next(seg);
  }

  if (seg->prev && !seg->prev->used) {
    // Previous segment is also free, we should merge them
    merge_segment_with_next(seg->prev);
    seg = NULL;
  }
}

void im_get_info(void (*callback)(void*, size_t, int)) {
  im_segment* cur = begin;
  while (cur) {
    callback(cur->buffer, cur->length, cur->used);
    cur = next_segment(cur);
  }
}
