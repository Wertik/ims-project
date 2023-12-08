#pragma once

#include "simulation.h"
#include "util.h"

typedef struct {
    // average time until the car found a parking spot
    float avg_until_parked;
    // average time until the car left the parking lot after leaving the parking spot
    float avg_until_leave;
    // average time waiting at an intersection
    float avg_inter_wait;
    // percentual representation of how many cars left without parking
    float perc_left_without_park;
} stats_t;

// calculate the statistics

stats_t *create_stats();
void free_stats(stats_t *stats);

stats_t *calculate_stats(simulation_data_t *data);
void print_stats(stats_t *stats);
