#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
  IDX_UP = 0,
  IDX_DOWN,
  IDX_RIGHT,
  IDX_LEFT,
  IDX_COUNT,
} dir_index_e;

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

position_t add_dir(position_t pos, dir_index_e direction);
position_t set_dir(position_t pos, dir_index_e direction);

void print_pos(position_t pos);
