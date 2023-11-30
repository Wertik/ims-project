#pragma once

#define MERROR(VAR)       \
  do {                    \
    if (VAR == NULL) {    \
      perror(__func__);   \
      exit(EXIT_FAILURE); \
    }                     \
  } while (0);
