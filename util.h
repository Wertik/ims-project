#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MERROR(VAR)       \
  do {                    \
    if (VAR == NULL) {    \
      perror(__func__);   \
      exit(EXIT_FAILURE); \
    }                     \
  } while (0);

#define BTS(VAL) (VAL ? "yes" : "no")

#define IS_ENV(NAME)       \
  (getenv(NAME) != NULL && \
   (strcmp(getenv(NAME), "1") != 0 || strcmp(getenv(NAME), "true") != 0))

// if threre are no vargs, no ',' gets added
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
#define VARGS(...) , ##__VA_ARGS__

#define VERBOSE(FMT, ...)              \
  do {                                 \
    if (IS_ENV("DEBUG")) {             \
      verbose(FMT VARGS(__VA_ARGS__)); \
    }                                  \
  } while (0);

// verbose helper fn to allow empty vargs
void verbose(const char* fmt, ...);
