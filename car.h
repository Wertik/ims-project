#pragma once

#include "pos.h"
#include "road.h"

typedef struct {
  position_t pos;

  bool c_override;
  SDL_Color color;

  // previous position
  // - for intersection decisions (don't wanna go back)
  position_t p_pos;
  // speed per tick
  position_t speed;

  // spawned at what tick?
  int spawned_at;

  // parked?
  bool parked;
  // at what tick the car parked
  int parked_at;

  // waiting on intersection?
  bool waiting;

  // leaving the map, looking for an exit
  bool leaving;

  // car is finished with the simulation
  bool left;
  // when did the car leave
  int left_at;

  // navigation through the whole map
  // aka what direction to take on intersections
  direction_e *inter_nav;
  int inter_nav_count;

  // immediate navigation steps
  // aka what directions to take right now
  direction_e *nav;
  int nav_count;
} car_t;

typedef struct {
    int size;
    car_t **data;
} car_list_t;

car_list_t *create_car_list();
void add_car(car_list_t *list, car_t *car);
void rem_car(car_list_t *list, car_t *car);

car_t *get_car(car_list_t *list, position_t pos);
car_t **get_cars_around(car_list_t *list, position_t pos);

void free_car_list(car_list_t *list);

car_t *create_car(position_t pos);
void free_car(car_t *car);

// print information about a car
void print_car(car_t *car, bool nl);

void add_nav_steps(car_t *car, direction_e steps[], int count);
direction_e get_nav_step(car_t *car);

// pop a navigation step
direction_e pop_nav_step(car_t *car);
