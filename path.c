#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "builder.h"
#include "car.h"
#include "graph.h"
#include "simulation.h"
#include "stats.h"

extern char *optarg;
extern int optind, opterr, optopt;

bool run(simulation_data_t *data) {
  run_inters(data);

  bool cars_left = run_cars(data);

  // run generators last to draw freshly generated cars before they move
  bool cars_to_generate = run_generators(data);

  // run the simulation until all cars left and there are still cars to generate
  return !cars_left || cars_to_generate;
}

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

void start_graph(simulation_data_t *data, int sim_speed) {
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  bool quit = false;

  // initial draw in case the simulation is paused
  initialize_SDL(&window, &renderer);
  draw(renderer, data);

  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        data->paused = !data->paused;
      }

      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d) {
        // -- print information about cars

        VERBOSE("-- Cars (%d)\n", data->cars->size);
        for (int i = 0; i < data->cars->size; i++) {
          print_car(data->cars->data[i], true);
        }
      }

      // left click prints the location of the cell
      if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;

        // get the cell coordinates

        unsigned int pos_x = x / CELL_SIZE;
        unsigned int pos_y = y / CELL_SIZE;

        VERBOSE("Clicked at [%d;%d]\n", pos_x, pos_y);

        // Print all the entities on this location

        for (unsigned int i = 0; i < data->entities->size; i++) {
          entity_t *e = data->entities->entities[i];

          if (e->pos.x == pos_x && e->pos.y == pos_y) {
            print_entity(e, true);
          }
        }

        // print cars

        for (int i = 0; i < data->cars->size; i++) {
          car_t *car = data->cars->data[i];

          if (car->pos.x == pos_x && car->pos.y == pos_y) {
            print_car(car, true);
          }
        }

        // print road info
        // assume they don't overlap

        road_t *road = get_road(data->roads, (position_t){pos_x, pos_y});

        if (road != NULL) {
          print_road(road, true);
        }

        // print intersection info
        // (only the core info)

        inter_t *inter =
            get_inter(data->intersections, (position_t){pos_x, pos_y});

        if (inter != NULL) {
          print_inter(inter, true);
        }
      }
    }

    // no pause when running without GUI - no way to unpause
    if (data->paused) {
      continue;
    }

    VERBOSE("--- Tick #%d\n", data->tick);

    bool should_quit = !run(data);

    draw(renderer, data);

    if (sim_speed != 0) {
      SDL_Delay(sim_speed);
    }

    VERBOSE("---\n");
    data->tick += 1;

    // run for at least 4 ticks
    // - wait for car generators
    if (quit || should_quit) {
      VERBOSE("Stopping...\n");
      // Pauza pro zobrazení výsledků
      SDL_Delay(1000);
      quit = true;
      continue;
    }
  }

  close_SDL(window, renderer);
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
    start_graph(&data, sim_speed);
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
  map_e map = SINGLE_INTER;
  bool start_paused = false;
  int sim_speed = 400;
  bool graph = true;
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
            "-l run without a graphical interface\n"
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
        graph = false;
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
