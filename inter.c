#include "inter.h"

inter_list_t *create_inter_list() {
  inter_list_t *list = (inter_list_t *)malloc(sizeof(inter_list_t));
  MERROR(list);

  list->data = NULL;
  list->size = 0;
  return list;
}

void add_inter(inter_list_t *list, inter_t *inter) {
  list->data =
      (inter_t **)realloc(list->data, sizeof(inter_t *) * (list->size + 1));
  MERROR(list->data);

  list->data[list->size] = inter;
  list->size += 1;
}

void free_inter_list(inter_list_t *list) {
  for (int i = 0; i < list->size; i++) {
    free_inter(list->data[i]);
  }

  free(list->data);
  free(list);
}

inter_t *create_inter() {
  inter_t *inter = (inter_t *)malloc(sizeof(inter_t));
  MERROR(inter);

  inter->part_count = 0;
  inter->parts = NULL;
  inter->wait_count = 0;

  for (int dir = DIR_UP; dir < DIR_COUNT; dir++) {
    inter->wait_spots[dir] = NULL;
    inter->wait_cars[dir] = NULL;
  }

  return inter;
}

void add_inter_part(inter_t *inter, e_road_t *part) {
  inter->parts = (e_road_t **)realloc(
      inter->parts, sizeof(e_road_t *) * (inter->part_count + 1));
  MERROR(inter->parts);

  inter->parts[inter->part_count] = part;
  inter->part_count += 1;
}

void add_inter_wait_spot(inter_t *inter, direction_e direction,
                         e_road_t *wait_spot) {
  inter->wait_spots[direction] = wait_spot;
}

void free_inter(inter_t *inter) {
  free(inter->parts);
  free(inter);
}

// get intersection by parts
inter_t *get_inter(inter_list_t *list, position_t pos) {
  for (int i = 0; i < list->size; i++) {
    inter_t *inter = list->data[i];

    for (int j = 0; j < inter->part_count; j++) {
      e_road_t *part = inter->parts[i];

      if (part->pos.x == pos.x && part->pos.y == pos.y) {
        return inter;
      }
    }
  }
  return NULL;
}

// get intersection by waiting spots
inter_t *get_inter_wait(inter_list_t *list, position_t pos) {
  for (int i = 0; i < list->size; i++) {
    inter_t *inter = list->data[i];

    for (int dir = DIR_UP; dir < DIR_COUNT; dir++) {
      e_road_t *spot = inter->wait_spots[dir];

      if (spot == NULL) {
        continue;
      }

      if (spot->pos.x == pos.x && spot->pos.y == pos.y) {
        return inter;
      }
    }
  }
  return NULL;
}

direction_e get_inter_wait_dir(inter_t *inter, position_t pos) {
  for (int dir = DIR_UP; dir < DIR_COUNT; dir++) {
    e_road_t *spot = inter->wait_spots[dir];

    if (spot == NULL) {
      continue;
    }

    if (spot->pos.x == pos.x && spot->pos.y == pos.y) {
      return dir;
    }
  }
  return DIR_COUNT;
}

void add_car_wait_spot(inter_t *inter, direction_e dir, car_t *car) {
  inter->wait_cars[dir] = car;
  inter->wait_count += 1;
}

void rem_car_wait_spot(inter_t *inter, direction_e dir) {
  inter->wait_cars[dir] = NULL;
  inter->wait_count -= 1;
}
