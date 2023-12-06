#include "pos.h"

cord_t min(cord_t a, cord_t b) { return a < b ? a : b; }

cord_t max(cord_t a, cord_t b) { return a > b ? a : b; }

cord_t bounds(cord_t a, cord_t b_min, cord_t b_max) {
  return min(max(a, b_min), b_max);
}

bool cmp_pos(position_t a, position_t b) { return a.x == b.x && a.y == b.y; }

void print_pos(position_t pos) { printf("[%d;%d]\n", pos.x, pos.y); }

direction_e inverse_dir(direction_e dir) {
  // direction enum not done properly, have to use a switch here
  switch (dir) {
    case DIR_UP:
      return DIR_DOWN;
    case DIR_DOWN:
      return DIR_UP;
    case DIR_RIGHT:
      return DIR_LEFT;
    case DIR_LEFT:
      return DIR_RIGHT;
    case DIR_COUNT:
      printf("inverse_dir: invalid direction index.\n");
      exit(EXIT_FAILURE);
  }
  // should never occur
  return DIR_COUNT;
}

position_t add_dir(position_t pos, direction_e direction) {
  switch (direction) {
    case DIR_UP:
      pos.y += -1;
      break;
    case DIR_DOWN:
      pos.y += 1;
      break;
    case DIR_RIGHT:
      pos.x += +1;
      break;
    case DIR_LEFT:
      pos.x += -1;
      break;
    case DIR_COUNT:
      printf("Invalid direction index.\n");
      exit(EXIT_FAILURE);
  }
  return pos;
}

position_t set_dir(position_t pos, direction_e direction) {
  pos.x = 0;
  pos.y = 0;
  switch (direction) {
    case DIR_UP:
      pos.y = -1;
      break;
    case DIR_DOWN:
      pos.y = 1;
      break;
    case DIR_RIGHT:
      pos.x = +1;
      break;
    case DIR_LEFT:
      pos.x = -1;
      break;
    case DIR_COUNT:
      printf("Invalid direction index.\n");
      exit(EXIT_FAILURE);
  }
  return pos;
}
