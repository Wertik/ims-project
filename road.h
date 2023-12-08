#pragma once

#include <stdbool.h>

#include "entity.h"
#include "pos.h"

// a road consists of multiple empty_road entities ina straight line
// it dictates a direction for cars
typedef struct {
  // parts, entities that make up the road
  e_road_t **parts;
  int part_count;

  bool has_exit;
} road_t;

typedef struct {
  road_t **roads;
  int size;
} road_list_t;

road_list_t *create_road_list();
void add_road(road_list_t *list, road_t *road);
void free_road_list(road_list_t *list);

// create a road from multiple empty_road parts
road_t *create_road();

bool is_part(road_t *road, e_road_t *part);
void add_part(road_t *road, e_road_t *part);

void free_road(road_t *road);

road_t *get_road(road_list_t *list, position_t pos);

void print_road(road_t *road, bool nl);
