#pragma once

#include "road.h"
#include "pos.h"
#include "simulation.h"

// car navigation logic

bool run_cars(simulation_data_t *data);
void run_car(simulation_data_t *data, car_t *car);

void move_car(entity_list_t *list, car_t *car);
