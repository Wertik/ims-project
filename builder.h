#pragma once

#include "inter.h"
#include "pos.h"
#include "simulation.h"

// build a road
// POSITIONS has to be guarded by the ARR() macro if an array literal is used
#define BUILD_ROAD(DATA, POSITIONS, DIR)                \
  do {                                                  \
    position_t positions[] = POSITIONS;                 \
                                                        \
    int count = sizeof(positions) / sizeof(position_t); \
                                                        \
    build_road(data, positions, count, DIR);            \
  } while (0);

// To pass an array literal as an argument for macros
// https://stackoverflow.com/a/5504336/20205862
#define ARR(...) __VA_ARGS__

// map builder

typedef struct {
  position_t pos;
  direction_e dir;
} inter_spot_data_t;

// different maps
typedef enum {
  SINGLE_INTER = 0,
  ROAD_DIR,
  MULTI_ROAD,
  MULTI_INTER,
  PARKING_LOT,
  MAP_COUNT,
} map_e;

void build_map(simulation_data_t *data, map_e map);

void generate_cars(simulation_data_t *data);