#include "istd/except.h"
#include "istd/mini-test.h"
#include <stdbool.h>

imini_test_case("istd : Exceptions") {
  
  bool body = false, after_throw = false;

  itry {
    body = true;
    iexcept_throw(&body, NULL, NULL);
    after_throw = true;
  } icatch(e) {
    imini_test_assert_equal(e, &body, "Catch should get thrown pointer");
  }

  imini_test_assert(body, "Body of exception should be called");
  imini_test_assert(!after_throw, "Throw should interrupt normal flow");
}

