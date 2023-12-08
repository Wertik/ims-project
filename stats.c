#include "stats.h"

stats_t *create_stats() {
  stats_t *stats = (stats_t *)malloc(sizeof(stats_t));
  MERROR(stats);

  stats->avg_inter_wait = 0;
  stats->avg_until_leave = 0;
  stats->avg_until_parked = 0;
  return stats;
}

void free_stats(stats_t *stats) { free(stats); }

stats_t *calculate_stats(simulation_data_t *data) {
  // take the cars that left the simulation and calculate statistics
  stats_t *stats = create_stats();

  int car_count = data->cars_left->size;

  // -- avg until parked
  int until_parked_sum = 0;
  int until_left_sum = 0;
  for (int i = 0; i < car_count; i++) {
    car_t *car = data->cars_left->data[i];

    // some cars haven't parked
    if (car->parked_at != -1) {
      until_parked_sum += car->parked_at - car->spawned_at;
    }

    if (car->left_at != -1) {
      until_left_sum += car->left_at - car->parked_at;
    }
  }
  stats->avg_until_parked = until_parked_sum / (float)car_count;
  stats->avg_until_leave = until_left_sum / (float)car_count;

  return stats;
}

void print_stats(stats_t *stats) {
  printf("avg_until_parked=%.03f\n", stats->avg_until_parked);
  printf("avg_until_leave=%.03f\n", stats->avg_until_leave);
  printf("avg_inter_wait=%.03f\n", stats->avg_inter_wait);
}
