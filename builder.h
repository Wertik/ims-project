#pragma once

#include "simulation.h"
#include "pos.h"
#include "inter.h"

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
    MAP_COUNT,
} map_e;

void build_map(simulation_data_t *data, map_e map);
