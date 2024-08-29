#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "istd/rle.h"

static void go_read_manual() {
  fprintf(stderr, "Go read the manual (in this case --help)");
  exit(-1);
}

static void the_manual() {
  fprintf(stderr, "rle-tool : A tool to encode anything in RLE and decode it back\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  x|extract FILE OUTPUT : extract given file\n");
  fprintf(stderr, "  c|create  FILE OUTPUT : compress file\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "You can write `-` in place of file to use stdin/stdout\n");
  fprintf(stderr, "\n");
  exit(-1);
}

#define MODE_INPUT 1
#define MODE_OUTPUT 2

static FILE* open_or_fail(const char* fname, int mode) {
  if (!strcmp(fname, "-")) {
    switch(mode) {
      case MODE_INPUT: return stdin;
      case MODE_OUTPUT: return stdout;
      default: assert(0);
    }
  }
  FILE* f = fopen(fname, mode == MODE_INPUT ? "r" : "w");
  if (!f) {
    fprintf(stderr, "Failed to open %s: %s", fname, strerror(errno));
    exit(-1);
  }
  return f;
}

int main (int argc, char** argv) {
  if (argc == 1)
    go_read_manual();

  if (!strcmp(argv[1], "--help"))
    the_manual();

  if (!strcmp(argv[1], "x") || !strcmp(argv[1], "extract") ||
      !strcmp(argv[1], "c") || !strcmp(argv[1], "create")) {

    if (argc != 4)
      go_read_manual();

    FILE* input = open_or_fail(argv[2], MODE_INPUT),
        * output = open_or_fail(argv[3], MODE_OUTPUT);
    
    char* buf = NULL;
    size_t len = 0, avail = 0;
    int in = fgetc(input);

    while (in != EOF) {
      if (len + 1 > avail) {
        avail = avail * 3 / 2 + 2;
        buf = realloc(buf, avail);
      }
      buf[len++] = (char) in;
      in = fgetc(input);
    }

    char* output_data;
    size_t out_len;

    if(!strcmp(argv[1], "x") || !strcmp(argv[1], "extract")) {
      out_len = istd_rle_decoded_length(buf, len);
      output_data = (char*) calloc(out_len, 1);

      if (istd_rle_decode(buf, len, output_data)) {
        fprintf(stderr, "Congratulations, you got a corrupted archive!\n");
        exit(-1);
      }

      fprintf(stderr, "Decompressed %zu bytes\n", out_len);
    } else {
      out_len = istd_rle_encoded_length(buf, len);
      output_data = (char*) calloc(out_len, 1);
      istd_rle_encode(buf, len, output_data);

      fprintf(stderr, "%zu bytes in, %zu bytes out - compression factor of %f\n",
              len, out_len, (double) len / (double) out_len);
    }

    fwrite(output_data, 1, out_len, output);

    free(buf);
    free(output_data);
    if (input != stdin) fclose(input);
    if (output == stdout) fclose(output); 
  } else {
    go_read_manual();
  }
}
