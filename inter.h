#pragma once

#include "pos.h"
#include "road.h"

// an intersection
// - roads that lead into and from this intersection
// - queues for disting incoming roads
// - paths through the intersection (?)

typedef struct {
  // parts of this intersection
  e_road_t **parts;
  int part_count;

  // queue places (aka cells where the car waits at the intersection)
  // [direction] -> [queue road cell]
  e_road_t *wait_spots[DIR_COUNT];
} inter_t;

typedef struct {
  inter_t **data;
  int size;
} inter_list_t;

inter_list_t *create_inter_list();
void add_inter(inter_list_t *list, inter_t *inter);
void free_inter_list(inter_list_t *list);

inter_t *create_inter();
void add_inter_part(inter_t *inter, e_road_t *part);
void add_inter_wait_spot(inter_t *inter, direction_e direction,
                         e_road_t *wait_spot);
void free_inter(inter_t *inter);

// get intersection by parts
inter_t *get_inter(inter_list_t *list, position_t pos);

// get intersection by waiting spots
inter_t *get_inter_wait(inter_list_t *list, position_t pos);
