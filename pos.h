#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

typedef enum {
  DIR_UP = 0,
  DIR_DOWN,
  DIR_RIGHT,
  DIR_LEFT,
  // serves to indicate the count of directions
  // as well as an invalid/unser direction
  DIR_COUNT,
} direction_e;

// coordinate type, unsigned
typedef unsigned int cord_t;

typedef struct {
  cord_t x;
  cord_t y;
} position_t;

cord_t min(cord_t a, cord_t b);
cord_t max(cord_t a, cord_t b);

// clip value to bounds
cord_t bounds(cord_t a, cord_t min, cord_t max);

bool cmp_pos(position_t a, position_t b);

position_t add_dir(position_t pos, direction_e direction);
position_t set_dir(position_t pos, direction_e direction);

direction_e inverse_dir(direction_e dir);

void print_pos(position_t pos);

direction_e *get_nav(position_t from, position_t to, bool y_prio, int *count);
