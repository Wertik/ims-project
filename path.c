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
  bool run_sim = run_cars(data);

  // run generators last to draw freshly generated cars before they move
  run_generators(data);

  return run_sim;
}

void start_headless(simulation_data_t *data, int sim_speed) {
  bool quit = false;

  while (!quit) {
    VERBOSE("--- Tick #%d\n", data->tick);

    bool should_quit = !run(data);

    usleep(sim_speed * 1000);

    VERBOSE("---\n");
    data->tick += 1;

    // run for at least 4 ticks
    // - wait for car generators
    if (quit || (should_quit && data->tick > 4)) {
      VERBOSE("Stopping...\n");
      // Pauza pro zobrazení výsledků
      usleep(1000000);
      quit = true;
      continue;
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
      }
    }

    // no pause when running without GUI - no way to unpause
    if (data->paused) {
      continue;
    }

    VERBOSE("--- Tick #%d\n", data->tick);

    bool should_quit = !run(data);

    draw(renderer, data);

    SDL_Delay(sim_speed);

    VERBOSE("---\n");
    data->tick += 1;

    // run for at least 4 ticks
    // - wait for car generators
    if (quit || (should_quit && data->tick > 4)) {
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
                          int sim_speed) {
  simulation_data_t data = {.roads = create_road_list(),
                            .entities = create_entity_list(),
                            .intersections = create_inter_list(),
                            .generators = create_gen_list(),
                            .cars = create_car_list(),
                            .cars_left = create_car_list(),
                            .tick = 0,
                            .paused = start_paused};

  build_map(&data, map);

  // init random

  srand(time(NULL));

  // run the simulation
  if (graph == true) {
    start_graph(&data, sim_speed);
  } else {
    start_headless(&data, sim_speed);
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

  while ((opt = getopt(argc, argv, "phm:s:l")) != -1) {
    switch (opt) {
      case 'h':
        printf(
            "Usage: ./path [-h] [-p] [-m MAP] [-s SPEED]\n\n"
            "-h get help\n"
            "-p start the simulation paused\n"
            "-m MAP id of map to use for the simulation (enum map_e)\n"
            "-s SPEED time in ms to wait each tick\n"
            "-l run without a graphical interface\n");
        return EXIT_SUCCESS;
      case 'm': {
        int m = atoi(optarg);
        if (m >= MAP_COUNT || m < 0) {
          printf("invalid map %s, range: <%d; %d>\n", optarg, 0, MAP_COUNT);
          return EXIT_FAILURE;
        }
        map = m;
        break;
      }
      case 's': {
        int s = atoi(optarg);
        if (s <= 0) {
          printf("cannot go under 1ms a tick.");
          return EXIT_FAILURE;
        }
        sim_speed = s;
        break;
      }
      case 'l':
        graph = false;
        break;
      case 'p':
        start_paused = true;
        break;
      case ':':
        printf("option needs a value\n");
        break;
      case '?':
        printf("unknown option : %c\n", optopt);
        break;
    }
  }

  // run the simulation!
  stats_t *stats = start_simulation(map, start_paused, graph, sim_speed);

  print_stats(stats);
  free_stats(stats);
  return 0;
}
