#include "istd/custom-str.h"
#include "istd/mini-test.h"
#include <stdlib.h>

imini_test_case("Custom string.h") {
  
  imini_test_header("Strchr()");

  char buf[22] = "Hello world!";
  char buf2[22] = "";

  imini_test_assert_equal(i_strchr(buf, ' '), buf+5, "Strchr should find given character");
  imini_test_assert_equal(i_strchr(buf, 'a'), NULL, "Strchr should return NULL if there is no matching char");
  imini_test_assert_equal(i_strchr(buf, '\0'), NULL, "Strchr should not check \\0");

  imini_test_header("Strlen()");

  imini_test_assert_equal(i_strlen(buf), 12, "Strlen should return sensible results");

  imini_test_header("Strcpy() / Strncpy()");

  i_strcpy(buf, buf2);

  imini_test_assert_str_equal(buf, buf2, "Strcpy should copy all the string");

  i_strncpy(buf, buf2, 3);
  imini_test_assert_str_equal(buf2, "He", "Strncpy should copy only N-1 characters");

  i_strcpy("foobar", buf2);

  imini_test_header("Strcat() / Strncat()");

  i_strcat(buf, buf2);
  imini_test_assert_str_equal(buf, "Hello world!foobar", "Strcat() should concat strings");

  i_strncat(buf, buf2, 22);
  imini_test_assert_str_equal(buf, "Hello world!foobarfoo", "Strncat() should respect given borders");

  imini_test_header("Strdup()");

  char* dup = i_strdup(buf2);

  imini_test_assert_str_equal(dup, buf2, "Strdup should copy given value");

  free(dup);  
}
