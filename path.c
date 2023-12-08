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

extern char *optarg;
extern int optind, opterr, optopt;

bool run(simulation_data_t *data) {
  run_inters(data);
  bool run_sim = run_cars(data);

  // run generators last to draw freshly generated cars before they move
  run_generators(data);

  return run_sim;
}

int main(int argc, char *argv[]) {
  // -- parse arguments

  int opt;

  bool start_paused = false;

  map_e map = SINGLE_INTER;

  while ((opt = getopt(argc, argv, "phm:")) != -1) {
    switch (opt) {
      case 'h':
        printf(
            "Usage: ./path [-h] [-p] [-m MAP]\n\n-h get help\n-p start the "
            "simulation "
            "paused\n-m MAP id of map to use for the simulation (enum "
            "map_e)\n");
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

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  initialize_SDL(&window, &renderer);

  simulation_data_t data = {.roads = create_road_list(),
                            .entities = create_entity_list(),
                            .intersections = create_inter_list(),
                            .generators = create_gen_list(),
                            .tick = 0};

  build_map(&data, map);

  // init random

  srand(time(NULL));

  bool quit = false;

  bool paused = start_paused;

  // initial draw in case the simulation is paused
  draw(renderer, &data);

  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        paused = !paused;
      }

      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d) {
        // d for debug, print information about the simulation
        entity_list_t *list = data.entities;

        // -- print information about cars

        int car_count = 0;
        for (unsigned int i = 0; i < list->size; i++) {
          if (list->entities[i]->type == CAR) {
            car_count++;
          }
        }

        printf("-- Cars (%d)\n", car_count);
        for (unsigned int i = 0; i < list->size; i++) {
          if (list->entities[i]->type == CAR) {
            print_car((car_t *)list->entities[i], true);
          }
        }
      }

      // left click prints the location of the cell
      if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int x = e.button.x;
        int y = e.button.y;

        // get the cell coordinates

        int pos_x = x / CELL_SIZE;
        int pos_y = y / CELL_SIZE;

        printf("Clicked at [%d;%d]\n", pos_x, pos_y);
      }
    }

    if (paused) {
      continue;
    }

    printf("--- Tick #%d\n", data.tick);

    bool should_quit = !run(&data);
    draw(renderer, &data);

    // Zpoždění pro lepší pozorování
    if (map == PARKING_LOT) {
      SDL_Delay(100);
    } else {
      SDL_Delay(400);
    }

    printf("---\n");
    data.tick += 1;

    // run for at least 4 ticks
    // - wait for car generators
    if (should_quit && data.tick > 4) {
      printf("Stopping...\n");
      // Pauza pro zobrazení výsledků
      SDL_Delay(1000);
      quit = true;
      continue;
    }
  }

  free_entity_list(data.entities);
  free_road_list(data.roads);
  free_inter_list(data.intersections);
  free_gen_list(data.generators);

  close_SDL(window, renderer);
  return 0;
}
