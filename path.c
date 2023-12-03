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
  return run_cars(data);
}

int main(int argc, char *argv[]) {
  // -- parse arguments

  int opt;

  bool start_paused = false;

  while ((opt = getopt(argc, argv, "ph")) != -1) {
    switch (opt) {
      case 'h':
        printf(
            "Usage: ./path [-h] [-p]\n\n-h get help\n-p start the simulation "
            "paused\n");
        return EXIT_SUCCESS;
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
                            .intersections = create_inter_list()};

  build_map(&data);

  // init random

  srand(time(NULL));

  bool quit = false;

  int tick = 0;

  bool do_run = !start_paused;

  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        do_run = !do_run;
      }
    }

    if (do_run) {
      printf("--- Tick #%d\n", tick++);

      bool should_quit = !run(&data);
      draw(renderer, &data);

      // Zpoždění pro lepší pozorování
      SDL_Delay(400);
      printf("---\n");

      if (should_quit) {
        printf("Stopping...\n");
        // Pauza pro zobrazení výsledků
        SDL_Delay(1000);
        quit = true;
        continue;
      }
    } else {
      draw(renderer, &data);
    }
  }

  free_entity_list(data.entities);
  free_road_list(data.roads);

  close_SDL(window, renderer);
  return 0;
}
