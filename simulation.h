#pragma once

#include <stdbool.h>

#include "entity.h"
#include "inter.h"
#include "road.h"
#include "car.h"
#include "generator.h"

// -- simulation settings

// grid settings
#define GRID_HEIGHT 32
#define GRID_WIDTH 32

#define GRID_SIZE GRID_WIDTH *GRID_HEIGHT

// real cell size
#define CELL_SIZE 20

// how many ticks a car should wait at the parking spot
#define CAR_PARKED_TICKS 10

typedef struct {
  road_list_t *roads;
  entity_list_t *entities;
  inter_list_t *intersections;
  gen_list_t *generators;
  car_list_t *cars;

  // cars that left the simulation
  // keep them for statistics
  car_list_t *cars_left;

  int tick;
  bool paused;
} simulation_data_t;

void run_generators(simulation_data_t *data);
void run_generator(simulation_data_t *data, generator_t *gen);

void run_inters(simulation_data_t *data);
void run_inter(simulation_data_t *data, inter_t *inter);

bool run_cars(simulation_data_t *data);
void run_car(simulation_data_t *data, car_t *car);

void move_car(simulation_data_t *data, car_t *car);
