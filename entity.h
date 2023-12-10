#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "pos.h"
#include "util.h"

typedef enum {
  EMPTY_ROAD,
  PARKING,
  MAP_EXIT,
} entity_type_e;

typedef struct {
  entity_type_e type;

  position_t pos;
} entity_t;

typedef struct {
  entity_type_e type;

  position_t pos;

  // forced road cell direction, if any
  direction_e direction;
} e_road_t;

typedef struct {
  unsigned int size;
  entity_t **entities;
} entity_list_t;

typedef entity_t *(*creator_fn_t)(position_t pos);

const char *pretty_type(entity_type_e type);

entity_list_t *create_entity_list();
void add_entity(entity_list_t *list, entity_t *entity);
void rem_entity(entity_list_t *list, entity_t *entity);

entity_t *get_entity(entity_list_t *list, position_t pos);
entity_t **get_surroundings(entity_list_t *list, position_t pos);

void del_entity(entity_list_t *list, unsigned int idx);
void free_entity_list(entity_list_t *list);

entity_t *create_entity(position_t pos);
void free_entity(entity_t *entity);

e_road_t *create_road_entity(position_t pos);
void free_road_entity(e_road_t *road);
void print_road_e(e_road_t *road, bool nl);
void print_entity(entity_t *e, bool nl);

void create_entities(entity_list_t *list, position_t positions[], int count,
                     creator_fn_t creator_fn);

entity_t *creator_road(position_t pos);
entity_t *creator_parking(position_t pos);
