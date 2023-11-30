#include "pos.h"

cord_t min(cord_t a, cord_t b) { return a < b ? a : b; }

cord_t max(cord_t a, cord_t b) { return a > b ? a : b; }

cord_t bounds(cord_t a, cord_t b_min, cord_t b_max) {
  return min(max(a, b_min), b_max);
}

bool cmp_pos(position_t a, position_t b) { return a.x == b.x && a.y == b.y; }

void print_pos(position_t pos) { printf("[%d;%d]\n", pos.x, pos.y); }

position_t add_dir(position_t pos, dir_index_e direction) {
  switch (direction) {
    case IDX_UP:
      pos.y += -1;
      break;
    case IDX_DOWN:
      pos.y += 1;
      break;
    case IDX_RIGHT:
      pos.x += +1;
      break;
    case IDX_LEFT:
      pos.x += -1;
      break;
    case IDX_COUNT:
      printf("Invalid direction index.\n");
      exit(EXIT_FAILURE);
  }
  return pos;
}

position_t set_dir(position_t pos, dir_index_e direction) {
  pos.x = 0;
  pos.y = 0;
  switch (direction) {
    case IDX_UP:
      pos.y = -1;
      break;
    case IDX_DOWN:
      pos.y = 1;
      break;
    case IDX_RIGHT:
      pos.x = +1;
      break;
    case IDX_LEFT:
      pos.x = -1;
      break;
    case IDX_COUNT:
      printf("Invalid direction index.\n");
      exit(EXIT_FAILURE);
  }
  return pos;
}
