#pragma once

#include "entity.h"
#include "road.h"
#include "inter.h"

typedef struct {
  road_list_t *roads;
  entity_list_t *entities;
  inter_list_t *intersections;
} simulation_data_t;
