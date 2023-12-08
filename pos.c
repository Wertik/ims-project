#include "pos.h"

cord_t min(cord_t a, cord_t b) { return a < b ? a : b; }

cord_t max(cord_t a, cord_t b) { return a > b ? a : b; }

cord_t bounds(cord_t a, cord_t b_min, cord_t b_max) {
  return min(max(a, b_min), b_max);
}

bool cmp_pos(position_t a, position_t b) { return a.x == b.x && a.y == b.y; }

void print_pos(position_t pos) { VERBOSE("[%d;%d]\n", pos.x, pos.y); }

direction_e *get_nav_x(int delta_x, direction_e *res, int *count) {
  for (int x = 0; x < abs(delta_x); x += 1) {
    *count = *count + 1;
    res = (direction_e *)realloc(res, sizeof(direction_e) * (*count));
    MERROR(res);

    res[(*count) - 1] = delta_x > 0 ? DIR_RIGHT : DIR_LEFT;
  }
  return res;
}

direction_e *get_nav_y(int delta_y, direction_e *res, int *count) {
  for (int y = 0; y < abs(delta_y); y += 1) {
    *count = *count + 1;
    res = (direction_e *)realloc(res, sizeof(direction_e) * (*count));
    MERROR(res);

    res[(*count) - 1] = delta_y > 0 ? DIR_DOWN : DIR_UP;
  }
  return res;
}

direction_e *get_nav(position_t from, position_t to, bool y_prio, int *count) {
  int delta_x = to.x - from.x;
  int delta_y = to.y - from.y;

  direction_e *res = NULL;
  *count = 0;

  // negative - left, up
  // positive - right, down

  if (y_prio) {
    res = get_nav_y(delta_y, res, count);
    res = get_nav_x(delta_x, res, count);
  } else {
    res = get_nav_x(delta_x, res, count);
    res = get_nav_y(delta_y, res, count);
  }

  return res;
}

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
      fprintf(stderr, "inverse_dir: invalid direction index.\n");
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
      fprintf(stderr, "Invalid direction index.\n");
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
      fprintf(stderr, "Invalid direction index.\n");
      exit(EXIT_FAILURE);
  }
  return pos;
}
