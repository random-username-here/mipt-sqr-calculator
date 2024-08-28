/**
 * \file
 * \brief Run-length encoding algorithm
 *
 * ## How to use it?
 *
 * ```c
 * // We start with some data we will be encoding 
 * void* my_data = ...;
 * size_t my_data_length = ...;
 *
 * size_t encoded_length = istd_rle_encoded_length(my_data, my_data_length);
 * void* encoded = (void*) calloc(encoded_length, 1);
 * // ... check if we got NULL
 * istd_rle_encode(my_data, my_data_length, encoded);
 * 
 * // ... send encoded and encoded_length, or store, or something...
 *
 * size_t decoded_length = istd_rle_decoded_length(encoded, encoded_length);
 * if (decoded_length == ISTD_RLE_TOO_BIG ||
 *     decoded_length > some sensible size)
 *       // ... We got something big, handle it.
 * void* decoded = calloc(1, decoded_length);
 * /// ... Check if calloc() returned NULL again
 * int error = istd_rle_decode(encoded, encoded_length, decoded);
 * if (error) { // error = ISTD_RLE_INVALID
 *    // We have wrong data.
 * }
 *
 * // ... Now we decoded that. Do something with it.
 *
 * ```
 *
 * ## How does it work?
 *
 * First of all - RLE is an encoding in which we encode
 * sequences of repeating characters as `N x some character`.
 * In the simplest case, when we are encoding latin letters,
 * we can do something like this:
 *
 * ```py
 * rle_encode("AAABBBBC") -> "3 A, 4 B, 1 C"
 * ```
 *
 * But because we are encoding binary data and are hoping to
 * achive at least some sort of compression, we must abandon
 * nicely-written text format and use binary format.
 *
 * The simplest thing would be to do something like this:
 *
 * ```c
 * struct encoded_byte {
 *   char ch;        // The byte
 *   size_t repeats; // How many times we repeat it
 * };
 *
 * struct encoded_byte encoded_data[] = ...;
 * ```
 *
 * But then we would get 5x more bytes for text with not 
 * a lot of repeating symbols. Not nice.
 *
 * So, what do we do? We use less bytes for length of repeating
 * sequence. One way to do that is to use variable-length numbers.
 * For example, you can check how UTF8 encoding works.
 *
 * But that still doubles size of each non-repeating byte...
 *
 * So, what to do? We can use RLE not all the time! Just chose one
 * value, which does not occur a lot of times (definetely not 0x00 or 0xff),
 * and make it a flag, which tells encoder what RLE encoded
 * sequence follows.
 *
 * If we have that character somewhere, we must escape it. I do this by
 * encoding that character as RLE of length 0.
 *
 * So, decoding looks like this:
 *
 * ```c
 * while (!has_ended(input)) {
 *   char c = getc(input);
 *   if (c != RLE_BEGIN_CHAR) {
 *     emit(1, c); // Just a normal character
 *   } else {
 *     char first_length_byte = getc(input);
 *     if (first_length_byte == 0) {
 *       emit(1, RLE_BEGIN_CHAR); // Escaped RLE_BEGIN_CHAR
 *     } else { // Real RLE
 *       ungetc(input, first_length_byte);
 *       size_t length = read_variable_int(input);
 *       char repeated = getc(input);
 *       emit(length, repeated);
 *     }
 *   }
 * }
 *
 * ```
 *
 * Sizes are encoded simular to buffer encoding used in between
 * Etherum servers:
 *
 * | Number                            | Encoding          |
 * |-----------------------------------|-------------------|
 * | 0 < x < 253                       | `bytes(x)`        |
 * | x needs 2 bytes or 253 <= x < 255 | `0xfd + bytes(x)` |
 * | x needs 4 bytes                   | `0xfe + bytes(x)` |
 * | x needs 8 bytes                   | `0xff + bytes(x)` |
 *
 * \note For now if x needs 8 bytes, we simply return `ISTD_RLE_TOO_BIG`,
 *       because `size_t` used cannot handle it.
 *
 * \todo Add some flag to prevent user from ignoring return values of
 *       `_length` functions.
 *
 * \todo Assure we are using some standard endianness
 */

#ifndef ISTD_RLE
#define ISTD_RLE

#include <stddef.h>

#ifndef ISTD_RLE_BEGIN_CHAR

  /// \brief RLE start character.
  ///
  /// Chosen to be somewhat random. Making this 0x00 is the
  /// worst idea ever.
  ///
  /// You can change it, if you want by defining your own
  /// value before including this file.
  ///  
  #define ISTD_RLE_BEGIN_CHAR ((char) (0x42))

#endif

/// \brief Value returned by functions returning sizes in case encoded/decoded
///
/// RLE size will be too big to fit into `size_t`.
///
#define ISTD_RLE_TOO_BIG ((size_t) (-1))

/// \brief Value returned by `istd_rle_decode()` if
///        we have invalid data
#define ISTD_RLE_INVALID ((int) 1)

/// \brief Encode buffer using RLE algorithm
///
/// \param [in]  buffer Buffer to encode
/// \param [in]  length Length of the encoded buffer
/// \param [out] output Target buffer to output into. Must have
///                     enough space to fit result into.
///
/// \note Minimum `target` size can be obtained with
///       `istd_rle_encoded_length()`.
///
void istd_rle_encode(const void* buffer, size_t length, void* output);


/// \brief Decode buffer using RLE algorithm
/// 
/// \param [in]  input  Buffer with data encoded by RLE
/// \param [in]  length Length of the input buffer
/// \param [out] buffer Buffer to put decoded data into.
///                     Must have enough space to write decoded data into.
///
/// \note Minimum `buffer` size can be obtained using
///       `istd_rle_decoded_length()`
///
int istd_rle_decode(const void* input, size_t length, void* buffer);


/// \brief Compute buffer length needed for encoding given buffer
size_t istd_rle_encoded_length(const void* buffer, size_t length);


/// \brief Compute length of decoded data.
/// 
/// \warning Allways check size of decoded data, because user can easily
///          give RLE, which will expand to 4 GB (more is not possible
///          when using `size_t`, but would be possible if `size_t` was bigger).
///
size_t istd_rle_decoded_length(const void* input, size_t length);

#endif // ISTD_RLE
