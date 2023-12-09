#pragma once

#include "inter.h"
#include "pos.h"
#include "simulation.h"
#include "generator.h"

// build a road
// POSITIONS has to be guarded by the ARR() macro if an array literal is used
#define BUILD_ROAD(DATA, POSITIONS, DIR)                \
  do {                                                  \
    position_t positions[] = POSITIONS;                 \
                                                        \
    int count = sizeof(positions) / sizeof(position_t); \
                                                        \
    build_road(data, positions, count, DIR, false);     \
  } while (0);

#define BUILD_EXIT_ROAD(DATA, POSITIONS, DIR)           \
  do {                                                  \
    position_t positions[] = POSITIONS;                 \
                                                        \
    int count = sizeof(positions) / sizeof(position_t); \
                                                        \
    build_road(data, positions, count, DIR, true);      \
  } while (0);

#define BUILD_INTER(DATA, POSITIONS, WAIT_SPOTS, OPTIONS)                   \
  do {                                                                      \
    position_t positions[] = POSITIONS;                                     \
    inter_spot_data_t wait_spots[] = WAIT_SPOTS;                            \
    inter_spot_data_t options[] = OPTIONS;                                  \
                                                                            \
    int count = sizeof(positions) / sizeof(position_t);                     \
    int wait_spot_count = sizeof(wait_spots) / sizeof(inter_spot_data_t);   \
    int option_count = sizeof(options) / sizeof(inter_spot_data_t);         \
                                                                            \
    build_intersection(data, positions, count, wait_spots, wait_spot_count, \
                       options, option_count);                              \
  } while (0);

#define BUILD_PARKING(DATA, POSITIONS)                                  \
  do {                                                                  \
    position_t positions[] = POSITIONS;                                 \
    int count = sizeof(positions) / sizeof(position_t);                 \
                                                                        \
    create_entities(data->entities, positions, count, creator_parking); \
  } while (0);

#define ADD_CARS(DATA, POSITIONS)                       \
  do {                                                  \
    position_t positions[] = POSITIONS;                 \
    int count = sizeof(positions) / sizeof(position_t); \
                                                        \
    for (int i = 0; i < count; i++) {                   \
      car_t *car = create_car(positions[i]);            \
      add_car(data->cars, car);                         \
    }                                                   \
  } while (0);

#define ADD_GENERATOR(DATA, POSITION, INTERVAL, COUNT)         \
  do {                                                         \
    position_t pos = (position_t)POSITION;                     \
    generator_t *generator = create_gen(pos, INTERVAL, COUNT); \
    add_generator(data->generators, generator);                \
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
  MULTI_INTER,
  PARKING_LOT_RIGHT,
  PARKING_LOT_MAIN,

  LEAVE_NAV,

  MAP_COUNT,
} map_e;

void build_map(simulation_data_t *data, map_e map, generator_conf_t *gen_conf);