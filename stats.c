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
  int without_park = 0;
  int inter_wait_sum = 0;
  for (int i = 0; i < car_count; i++) {
    car_t *car = data->cars_left->data[i];

    // some cars haven't parked
    if (car->parked_at != -1) {
      until_parked_sum += car->parked_at - car->spawned_at;
    } else {
      without_park += 1;
    }

    if (car->left_at != -1) {
      until_left_sum += car->left_at - car->parked_at;
    }

    inter_wait_sum += car->inter_wait_time;

    print_car(car, true);
  }
  stats->avg_until_parked = until_parked_sum / (float)car_count;
  stats->avg_until_leave = until_left_sum / (float)car_count;
  stats->perc_left_without_park = without_park / (float)car_count;
  stats->avg_inter_wait = inter_wait_sum / (float)car_count;

  return stats;
}

void print_stats(stats_t *stats, bool csv) {
  if (csv == true) {
    printf("%.03f,%.03f,%.03f,%.03f\n", stats->avg_until_parked,
           stats->avg_until_leave, stats->avg_inter_wait,
           stats->perc_left_without_park * 100.0);
  } else {
    printf("avg_until_parked=%.03f\n", stats->avg_until_parked);
    printf("avg_until_leave=%.03f\n", stats->avg_until_leave);
    printf("avg_inter_wait=%.03f\n", stats->avg_inter_wait);
    printf("perc_left_without_park=%.03f\n",
           stats->perc_left_without_park * 100.0);
  }
}
