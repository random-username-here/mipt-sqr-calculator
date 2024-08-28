#include <assert.h>
#include <stdint.h>
#include "istd/rle.h"

//---- Vaint ---------------------------------------------//

#define VAINT_2_BYTES ((uint8_t) 0xfd)
#define VAINT_4_BYTES ((uint8_t) 0xfe)
#define VAINT_8_BYTES ((uint8_t) 0xff)

#define SIZE_T_2_BYTES ((size_t) 0xffff - 1)


/// Length of size encoded as variable-length integer
/// See documentation for how to do that.
static size_t vaint_encoded_len(size_t v) {
  if (v < VAINT_2_BYTES)
    return 1;
  else if (v <= SIZE_T_2_BYTES)
    return 2 + 1;
  else
    return 4 + 1;
  // 8 is never returned
}


/// Decoded length variadic integer, starting at that pos
static size_t vaint_decoded_len(uint8_t c) {
  if      (c == VAINT_8_BYTES) return 8 + 1; // for `c`
  else if (c == VAINT_4_BYTES) return 4 + 1;
  else if (c == VAINT_2_BYTES) return 2 + 1;
  else                         return 1;
}

/// Decode vaint into `*out`.
/// Returns buffer position after decoded data, or
/// `NULL` if in reading we will go past the end.
static const uint8_t* vaint_decode(
    const uint8_t* buf,
    const uint8_t* end,
    size_t* out
  ) {
  
  assert(buf);
  assert(end);
  assert(buf < end);
  assert(out);

  size_t len = vaint_decoded_len(*buf);

  if (buf + len > end)
    return NULL;

  // FIXME: endianness
  if (len == 1)
    *out = *((const  uint8_t*) (buf  ));
  else if (len == 3)
    *out = *((const uint16_t*) (buf+1));
  else if (len == 5)
    *out = *((const uint32_t*) (buf+1));
  else
    *out = ISTD_RLE_TOO_BIG;

  return buf + len;
}

/// Write vaint into given buffer.
static uint8_t* vaint_encode(uint8_t* buf, size_t value) {

  assert(buf);

  size_t len = vaint_encoded_len(value);

  // FIXME: endiannes
  if (len == 1)
    *((uint8_t*) buf) = (uint8_t) value;
  else if (len == 3) {
    *buf = VAINT_2_BYTES;
    *((uint16_t*) (buf+1)) = (uint16_t) value;
  } else if (len == 5) {
    *buf = VAINT_4_BYTES;
    *((uint32_t*) (buf+1)) = (uint32_t) value;
  }

  return buf + len;
}

//---- RLE -----------------------------------------------//

/// How many chars do you need to repeat for RLE to make sense
#define MAKES_SENSE_TO_REPEAT ((size_t) 3)

void istd_rle_encode(const void* void_buffer,
                     size_t length, void* void_output) {
  
  assert(void_buffer);
  assert(void_output);

  const uint8_t* buffer = (const uint8_t*) void_buffer;
  uint8_t* output = void_output;

  for (size_t pos = 0; pos < length;) {

    uint8_t ch = buffer[pos++];
    size_t repeat = 1;

    while (pos < length && buffer[pos] == ch)
      ++pos, ++repeat;

    if (repeat < MAKES_SENSE_TO_REPEAT) {
      for (size_t i = 0; i < repeat; ++i) {
        *(output++) = ch;
        if (ch == (uint8_t) ISTD_RLE_BEGIN_CHAR)
          // Must escape that character
          *(output++) = (uint8_t) 0;
      }
    } else {
      // Repeated character
      *(output++) = (uint8_t) ISTD_RLE_BEGIN_CHAR;
      output = vaint_encode(output, repeat);
      *(output++) = ch;
    }
  }
}

size_t istd_rle_encoded_length(const void* void_buffer, size_t length) {
  
  assert(void_buffer);

  // Look to istd_rle_encode() for comments, it has the same code.

  const uint8_t* buffer = (const uint8_t*) void_buffer;

  size_t result = 0;

  for (size_t pos = 0; pos < length; ) {
    uint8_t ch = buffer[pos++];
    size_t repeat = 1;

    while (pos < length && buffer[pos] == ch)
      ++pos, ++repeat;

    if (repeat < MAKES_SENSE_TO_REPEAT)
      result += (ch == (uint8_t) ISTD_RLE_BEGIN_CHAR ? 2 : 1) * repeat;
    else 
      result += 1 + vaint_encoded_len(repeat) + 1;
  }

  return result;
}

int istd_rle_decode(const void* void_input,
                     size_t length, void* void_buffer) {
  
  assert(void_input);
  assert(void_buffer);

  const uint8_t* input = (const uint8_t*) void_input;
  uint8_t* buffer = (uint8_t*) void_buffer;
  const uint8_t* end = input + length;

  while (input < end) {
    uint8_t ch = *(input++);

    if (ch != (uint8_t) ISTD_RLE_BEGIN_CHAR) {
      *(buffer++) = ch;
    } else {

      if (*input == 0) {
        // Escaped RLE start character...
        *(buffer++) = (uint8_t) ISTD_RLE_BEGIN_CHAR;
        continue;
      }

      size_t repeat = 0;
      input = vaint_decode(input, end, &repeat);

      if (!input || // Vaint attempted to go past the end
          input == end) // or we are at the end, and we
                        // still need char to repeat 
        return ISTD_RLE_INVALID;
      
      uint8_t repeated = *(input++);

      // Write that.
      // TODO: use more efficient call
      for (size_t i = 0; i < repeat; ++i)
        *(buffer++) = repeated;
    }
  }

  return 0;
}

size_t istd_rle_decoded_length(const void* void_input, size_t length) {
  
  assert(void_input);

  const uint8_t* input = (const uint8_t*) void_input;
  const uint8_t* end = input + length;

  size_t len = 0;

  while (input < end) {
    uint8_t ch = *(input++);
    
    if (ch != (uint8_t) ISTD_RLE_BEGIN_CHAR)
      len += 1;
    else {

      if (*input == 0) { // Escaped begin char
        len += 1;
        continue;
      }

      size_t repeat = 0;
      input = vaint_decode(input, end, &repeat);
      input++; // skip character to repeat

      if (input == NULL || input > end)
        return 0; // Bad data

      len += repeat;
    }
  }

  return len;
}
