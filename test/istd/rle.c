#include "istd/rle.h"
#include "istd/mini-test.h"
#include "istd/xxd.h"
#include <stdlib.h>

imini_test_case("RLE library") {
  
  //--------------------------------------------------------------------
  imini_test_header("RLE encoding of <<bad>> string (with no repeats)");
  //--------------------------------------------------------------------

  {
    char string[] = "A long string which does not make sense to RLE encode.";
    size_t string_len = sizeof(string);

    size_t encoded_len = istd_rle_encoded_length(string, string_len);

    imini_test_assert_equal(encoded_len, string_len, "RLE encoding of <<bad>> string must be optimal");

    char encoded[sizeof(string)];
    istd_rle_encode(string, string_len, encoded);

    imini_test_assert_str_equal(encoded, string, "RLE encoding of <<bad>> string must be the string itself");

    size_t decoded_len = istd_rle_decoded_length(encoded, encoded_len);

    imini_test_assert_equal(decoded_len, string_len, "Decoded string must have the same length as it had before encoding");

    char decoded[sizeof(string)];
    int error = istd_rle_decode(encoded, encoded_len, decoded);
    
    imini_test_assert_equal(error, 0, "String should be decoded without errors");
    imini_test_assert_str_equal(decoded, string, "Decoded string should be equal to original");
  }

  //----------------------------------------------------------
  imini_test_header("String which is sensible to RLE encode");
  //----------------------------------------------------------
 
  {
    char string[] = "aaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbb";
    size_t string_len = sizeof(string);

    size_t encoded_len = istd_rle_encoded_length(string, string_len);

    imini_test_assert_le(encoded_len, string_len, "RLE encoding of repetitive string should be smaller");

    void* encoded = calloc(encoded_len, 1);
    istd_rle_encode(string, string_len, encoded);

    istd_xxd(encoded, (char*) encoded + encoded_len, "Encoded string");

    size_t decoded_len = istd_rle_decoded_length(encoded, encoded_len);  

    imini_test_assert_equal(decoded_len, string_len, "Decoded string must have the same size as original");
    
    char decoded[sizeof(string)];
    istd_rle_decode(encoded, encoded_len, decoded);

    imini_test_assert_str_equal(decoded, string, "Decoded string must be equal to original string");
 
    free(encoded);
  }
}
