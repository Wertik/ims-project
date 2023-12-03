#pragma once

#include "simulation.h"
#include "pos.h"
#include "inter.h"

// map builder

typedef struct {
    position_t pos;
    direction_e dir;
} inter_spot_data_t;

void build_map(simulation_data_t *data);
