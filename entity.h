#pragma once

#include <SDL2/SDL_pixels.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pos.h"
#include "util.h"

typedef enum {
  EMPTY_ROAD,
  CAR,
  PARKING,
} entity_type_e;

typedef struct {
  entity_type_e type;

  position_t pos;

  bool c_override;
  SDL_Color color;
} entity_t;

typedef struct {
  entity_type_e type;

  position_t pos;

  bool c_override;
  SDL_Color color;

  // previous position
  // - for intersection decisions (don't wanna go back)
  position_t p_pos;
  // speed per tick
  position_t speed;
  // parked?
  bool parked;
  // waiting on intersection?
  bool waiting;
  // no path options?
  bool blocked;
} car_t;

typedef struct {
  entity_type_e type;

  position_t pos;

  bool c_override;
  SDL_Color color;

  // forced road cell direction, if any
  direction_e direction;
} e_road_t;

typedef struct {
  unsigned int size;
  entity_t **entities;
} entity_list_t;

typedef entity_t *(*creator_fn_t)(position_t pos);

entity_list_t *create_entity_list();
void add_entity(entity_list_t *list, entity_t *entity);

entity_t *get_entity(entity_list_t *list, position_t pos);
entity_t **get_surroundings(entity_list_t *list, position_t pos);

void del_entity(entity_list_t *list, unsigned int idx);
void free_entity_list(entity_list_t *list);

entity_t *create_entity(position_t pos);
void free_entity(entity_t *entity);

e_road_t *create_road_entity(position_t pos);
void free_road_entity(e_road_t *road);

car_t *create_car(position_t pos);
void free_car(car_t *car);

// print information about a car
void print_car(car_t *car, bool nl);

void create_entities(entity_list_t *list, position_t positions[], int count,
                     creator_fn_t creator_fn);

entity_t *creator_road(position_t pos);
entity_t *creator_parking(position_t pos);
entity_t *creator_car(position_t pos);
