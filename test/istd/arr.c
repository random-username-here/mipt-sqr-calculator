#include "istd/arr.h"
#include "istd/mini-test.h"

imini_test_case("Array") {
  
  imini_test_header("Empty array");

  char* arr_one = ia_empty_array(char);

  imini_test_assert_ne(arr_one, NULL, "Given array must not be NULL");
  imini_test_assert_equal(ia_length(arr_one), 0u, "Length of empty array must be zero");
  imini_test_assert_ge(ia_availiable(arr_one), 0u, "There should be nonnegative availiable space in array");


  imini_test_header("Small string array");
  
  char* arr_two = ia_string_array(char, "Hello world!");

  imini_test_assert_ne(arr_two, NULL, "Array must exist");
  imini_test_assert_equal(ia_length(arr_two), 12u, "Array must have proper length");
  imini_test_assert_ge(ia_availiable(arr_two), 12u, "Array shoud have at least space to store data");
  imini_test_assert_str_equal(arr_two, "Hello world!", "Array must have expected contents");

  imini_test_header("Changing it - pop/push");

  ia_push(arr_two, '-');
  ia_push(arr_two, '@');

  imini_test_assert_str_equal(arr_two, "Hello world!-@", "Push() should append new items");

  ia_pop(arr_two);

  imini_test_assert_str_equal(arr_two, "Hello world!-", "Pop() should remove items");

  imini_test_header("Changing - join operations");

  ia_joinz(arr_one, "message");

  imini_test_assert_str_equal(arr_one, "message", "Joinz() should append strings");

  char msg[] = " test ";
  ia_joinn(arr_one, msg, 6);
  imini_test_assert_str_equal(arr_one, "message test ", "Joinn()");

  ia_join(arr_one, arr_two);

  imini_test_assert_str_equal(arr_one, "message test Hello world!-", "Join() should join arrays");

  ia_destroy(arr_one);
  ia_destroy(arr_two);
}


