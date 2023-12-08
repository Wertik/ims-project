#pragma once

#include "road.h"
#include "pos.h"
#include "simulation.h"

// car navigation logic

void run_generators(simulation_data_t *data);
void run_generator(simulation_data_t *data, generator_t *gen);

void run_inters(simulation_data_t *data);
void run_inter(simulation_data_t *data, inter_t *inter);

bool run_cars(simulation_data_t *data);
void run_car(simulation_data_t *data, car_t *car);

void move_car(simulation_data_t *data, car_t *car);
