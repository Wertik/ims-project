#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "builder.h"
#include "car.h"
#include "simulation.h"
#include "stats.h"

#ifdef GRAPH
#include "graph.h"
#endif

extern char *optarg;
extern int optind, opterr, optopt;

void start_headless(simulation_data_t *data, int sim_speed, int timeout) {
  bool quit = false;

  time_t timeout_at = time(NULL) + timeout;

  while (!quit) {
    VERBOSE("--- Tick #%d\n", data->tick);

    quit = !run(data);

    if (sim_speed != 0) {
      usleep(sim_speed * 1000);
    }

    VERBOSE("---\n");
    data->tick += 1;

    if (time(NULL) == timeout_at) {
      quit = true;
      fprintf(stderr,
              "Simulation reached the timeout of %ds, stopping it.\nResults "
              "are most likely unusable.\n",
              timeout);
    }

    if (quit) {
      VERBOSE("Stopping...\n");
      // Pauza pro zobrazení výsledků
      usleep(1000000);
      break;
    }
  }
}

// run the simulation and return statistics
stats_t *start_simulation(map_e map, bool start_paused, bool graph,
                          int sim_speed, int timeout,
                          generator_conf_t gen_conf) {
  simulation_data_t data = {.roads = create_road_list(),
                            .entities = create_entity_list(),
                            .intersections = create_inter_list(),
                            .generators = create_gen_list(),
                            .cars = create_car_list(),
                            .cars_left = create_car_list(),
                            .tick = 0,
                            .paused = start_paused};

  build_map(&data, map, &gen_conf);

  // init random

  srand(time(NULL));

  // run the simulation
  if (graph == true) {
#ifndef GRAPH
    fprintf(stderr,
            "Cannot run a graphical model. Binary not built with SDL2.\n");
    exit(EXIT_FAILURE);
#else
    start_graph(&data, sim_speed);
#endif
  } else {
    start_headless(&data, sim_speed, timeout);
  }

  // print final statistics
  stats_t *stats = calculate_stats(&data);

  // free memory
  free_entity_list(data.entities);
  free_road_list(data.roads);
  free_inter_list(data.intersections);
  free_gen_list(data.generators);
  free_car_list(data.cars);
  free_car_list(data.cars_left);

  return stats;
}

int main(int argc, char *argv[]) {
  // -- parse arguments

  int opt;

  // options
  map_e map = PARKING_LOT_RIGHT;
  bool start_paused = false;
  int sim_speed = 0;
  bool graph = false;
  bool csv = false;
  int runs = 1;
  int timeout = 30;

  generator_conf_t gen_conf = {.count = -1, .interval = -1};

  while ((opt = getopt(argc, argv, "phm:s:c:li:vr:t:")) != -1) {
    switch (opt) {
      case 'h':
        printf(
            "Usage: ./path [-h] [-p] [-m MAP] [-s SPEED]\n\n"
            "-h get help\n"
            "-p start the simulation paused\n"
            "-m MAP id of map to use for the simulation (enum map_e)\n"
            "-s SPEED time in ms to wait each tick\n"
            "-c COUNT number of cars to generate\n"
            "-i INTERVAL interval between car generator calls\n"
            "-l run with a graphical interface\n"
            "-v return the statistics in csv format\n"
            "-r RUNS how many runs of the simulation to do, statistics are "
            "averaged out\n"
            "-t TIMEOUT timeout in seconds when to stop the simulation\n");
        return EXIT_SUCCESS;
      case 'm': {
        int m = atoi(optarg);
        if (m >= MAP_COUNT || m < 0) {
          fprintf(stderr, "invalid map %s, range: <%d; %d>\n", optarg, 0,
                  MAP_COUNT);
          return EXIT_FAILURE;
        }
        map = m;
        break;
      }
      case 'v': {
        csv = true;
        break;
      }
      case 's': {
        int s = atoi(optarg);
        if (s < 0) {
          fprintf(stderr, "SPEED cannot be negative.\n");
          return EXIT_FAILURE;
        }
        sim_speed = s;
        break;
      }
      case 'r': {
        int r = atoi(optarg);
        if (r <= 0) {
          fprintf(stderr, "number of runs has to fall into <1;INT_MAX>.\n");
          return EXIT_FAILURE;
        }
        runs = r;
        break;
      }
      case 'c': {
        int c = atoi(optarg);
        if (c <= 0) {
          fprintf(stderr, "cannot go under 1 cars.\n");
          return EXIT_FAILURE;
        }
        gen_conf.count = c;
        break;
      }
      case 'i': {
        int i = atoi(optarg);
        if (i <= 0) {
          fprintf(stderr, "interval cannot be <1.\n");
          return EXIT_FAILURE;
        }
        gen_conf.interval = i;
        break;
      }
      case 't': {
        int t = atoi(optarg);
        if (t <= 0) {
          fprintf(stderr, "timeout should be in <1; MAX_INT>\n");
          return EXIT_FAILURE;
        }
        timeout = t;
        break;
      }
      case 'l':
        graph = true;
        break;
      case 'p':
        start_paused = true;
        break;
      case ':':
        fprintf(stderr, "option needs a value\n");
        break;
      case '?':
        fprintf(stderr, "unknown option : %c\n", optopt);
        break;
    }
  }

  stats_t *final_stats = create_stats();

  stats_t **stat_list = (stats_t **)malloc(sizeof(stats_t *) * runs);
  MERROR(stat_list);
  memset(stat_list, 0, sizeof(stats_t *) * runs);

  // run the simulation(s)!
  for (int run = 0; run < runs; run++) {
    VERBOSE("Start run #%d\n", run);
    stat_list[run] = start_simulation(map, start_paused, graph, sim_speed,
                                      timeout, gen_conf);
    VERBOSE("End run #%d\n", run);
  }

  // average the stats out
  float sum_avg_inter_wait = 0;
  float sum_avg_until_leave = 0;
  float sum_avg_until_parked = 0;
  float sum_perc_left_without_park = 0;
  for (int run = 0; run < runs; run++) {
    stats_t *stats = stat_list[run];
    if (stats == NULL) {
      // the run didn't finish
      break;
    }

    sum_avg_inter_wait += stats->avg_inter_wait;
    sum_avg_until_leave += stats->avg_until_leave;
    sum_avg_until_parked += stats->avg_until_parked;
    sum_perc_left_without_park += stats->perc_left_without_park;

    free_stats(stats);
  }
  free(stat_list);
  stat_list = NULL;

  final_stats->avg_inter_wait = sum_avg_inter_wait / (float)runs;
  final_stats->avg_until_leave = sum_avg_until_leave / (float)runs;
  final_stats->avg_until_parked = sum_avg_until_parked / (float)runs;
  final_stats->perc_left_without_park =
      sum_perc_left_without_park / (float)runs;

  print_stats(final_stats, csv);

  free_stats(final_stats);
  return 0;
}
