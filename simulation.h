#pragma once

#include <stdbool.h>

#include "entity.h"
#include "inter.h"
#include "road.h"
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
  int tick;
} simulation_data_t;
