#include "istd/imini-test.h"

imini_test_case("Assert checks")
  imini_test_assert(1, "One must count as true"); 
  imini_test_assert(0, "Zero shall fail test"); 
imini_test_case_end

imini_test_case("Assert equal checks")
  imini_test_assert_equal(42, 42, "When values are equal"); 
  imini_test_assert_equal(42.4, 3, "When values are not equal"); 
imini_test_case_end

imini_test_case("Assert somewhat equal checks")
  imini_test_assert_somewhat_equal(0.1 + 0.2, 0.3, 1e-5, "Floats are somewhat equal"); 
  imini_test_assert_somewhat_equal(0.1 + 0.2, 0.3, 0, "... but not perfectly!"); 
imini_test_case_end


int main () {
  // nothing
}
