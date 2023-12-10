#include "util.h"

void verbose(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

// Function for generating exponential numbers in the [0,1) range
double generate_exponential() { return -log(1.0 - (double)rand() / RAND_MAX); }
