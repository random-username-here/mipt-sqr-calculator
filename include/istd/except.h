/**
 * \file
 * \brief Exception handling for C
 *
 * Syntax for it is:
 * ```c
 *
 * struct myexception { int foobar; };
 *
 * itry {
 *   // ... code
 *   // Construct some exception object
 *   struct myexception* err = { .foobar = 43 };
 *   // Throw it
 *   iexcept_throw(err, &free_myexception, &print_myexception);
 * } icatch(e) {
 *   struct myexception* ex = (struct myexception*) e;
 *   // Got exception of given type
 *   printf("Got exception `myexception { foobar = %d }`\n", ex.foobar);
 * }
 * ```
 * 
 * Getting stack traces and such stuff is up to you.
 *
 * ## How does it work?
 *
 * During execution there are two stacks: one for 
 * catch expressions (named catch tree), second 
 * to store during handling of which expections this 
 * occured (snow ball).
 *
 * Macros expand into something like this:
 * 
 * ```c
 * // Try block
 * if (!setjmp(new_element_in_catch_tree())) {
 *    // ...
 *    // We throw something
 *    snowball_add_exception(data, free_data, print_data);
 *    longjmp(element_on_top_of_catch_tree);
 *    // We get back to the `if` condition, but it evaluates to false now
 * }
 * // Catch block, it is actually for loop
 * else {
 *   // We got exception
 *   catch_tree_pop()
 * }
 * for (void* exception = get_exception();
 *      exception;
 *      free_data(exception),
 *      exception = NULL) {
 *   // We handle exception here
 * }
 * ```
 */


#ifndef ISTD_EXCEPT
#define ISTD_EXCEPT

#include <stdlib.h>
#include <setjmp.h>
#include <stdlib.h>

#ifndef istd_except_malloc 
  /// Function to allocate buffer of given size
  #define istd_except_malloc(sz) malloc(sz)
#endif


#ifndef istd_except_free 
  /// Function to free buffer
  #define istd_except_free(buf) free(buf)
#endif

/// Printer to print information about exception
/// if it was not caught.
typedef void (*iexcept_printer)(void*);

/// Function to free exception.
typedef void (*iexcept_cleaner)(void*);

/// \brief Beginning of try-catch block.
#define itry if (({ \
    jmp_buf* buf = _iexcept_catch_tree_new(); \
    int res = setjmp(*buf); /* Starting from here there are two paths */ \
    /* else we longjmp-ed here */ \
    !res; \
  }))

/// \brief Catch thrown object
/// Neutralizes that exception, so it won't propogate further.
/// TODO: else {  remove_elem_from_tree() }, in case 
/// \param varname Pointer to thrown object will be availiable
///                as that variable
#define icatch(varname) else _iexcept_catch_tree_close(); for(\
      void* varname = _iexcept_thrown();\
      varname;\
      _iexcept_clean_exc(), varname = NULL \
    )

/// \brief Register function to be executed during unwind
///
/// So, if you've allocated something and want to `free()` it in case some
/// exception gets thrown. 
///
/// \warning **only free those buffers with `iexcept_free()` afterwards, 
///          or you'll get double free in case of exception.**
/// 
/// \param buffer  Buffer to free on unwind
/// \param free_fn Function to call to free that buffer.
//void iexcept_add_free(void* buffer, void (*free_fn)(void*));

/// \brief Free buffer which was registered by `iexcept_add_free`
/// \param buffer Buffer to free
//void iexcept_free(void* buffer);

//---- setjmp madness ------------------------------------//

// Because we cannot move jmp_buf-s, we must 
// allocate some place for them before calling setjmp.
// So we have a stack for that.

/// Create new jmpbuf in jump stack
jmp_buf* _iexcept_catch_tree_new();

/// Close item on jump stack
void _iexcept_catch_tree_close();

  /// Gets currently handled exception
void* _iexcept_thrown();

/// Current exception
void _iexcept_clean_exc();

/// Throw given exception
/// \param data - Exception data
/// \param clean - Function to free given data
/// \param print - Function to print that exception in case
///                it won't be caught.
///
/// Functions can be `NULL`, in that case:
///   - `clean()` - data is not freed by any means
///   - `print()` - Some message, like `<no printer specified for this exeption>`
///                 will be printed.
void iexcept_throw(void* data, iexcept_cleaner clean, iexcept_printer print);


#endif

