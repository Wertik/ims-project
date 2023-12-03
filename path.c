#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

#include "builder.h"
#include "entity.h"
#include "graph.h"
#include "road.h"
#include "simulation.h"

// move the car according to it's speed
void move_car(entity_list_t *list, car_t *car) {
  // act on speed, update the car position

  cord_t new_x = bounds(car->pos.x + car->speed.x, 0, GRID_WIDTH - 1);
  cord_t new_y = bounds(car->pos.y + car->speed.y, 0, GRID_HEIGHT - 1);

  if (new_x == car->pos.x && new_y == car->pos.y) {
    return;
  }

  // car can only move to it's destinated cell, if there is an empty road

  entity_t *dst = get_entity(list, (position_t){new_x, new_y});

  // nothing there, cannot move to void
  if (dst == NULL) {
    print_car(car, false);
    printf("cannot move forward, no road ahead.\n");
    return;
  }

  // cannot move into anything else
  if (dst->type == EMPTY_ROAD) {
    car->p_pos = car->pos;

    car->pos.x = new_x;
    car->pos.y = new_y;

    print_car(car, false);
    printf("moved forward.\n");
    return;
  }

  if (dst->type == PARKING) {
    car->parked = true;

    car->p_pos = car->pos;

    car->pos.x = new_x;
    car->pos.y = new_y;

    car->speed.x = 0;
    car->speed.y = 0;

    print_car(car, false);
    printf("reached a parking spot.\n");
    return;
  }

  print_car(car, false);
  printf("cannot move forward, blocked.\n");
  return;
}

// return true if the simulation should continue
bool run(simulation_data_t *simul_data) {
  entity_list_t *list = simul_data->entities;

  bool cars_parked = true;

  // move cars according to their speed
  for (unsigned int i = 0; i < list->size; i++) {
    entity_t *entity = list->entities[i];

    if (entity->type == CAR) {
      car_t *car = (car_t *)entity;

      printf("- ");
      print_car(car, true);

      // no need to do anything when parked
      if (car->parked) {
        cars_parked &= cars_parked;
        continue;
      }

      // car simulation logic

      cars_parked = false;

      // "path finding"

      // if there is a parking lot on the side of the road, head into it
      entity_t **around = get_surroundings(list, car->pos);

      // otherwise if we are on an intersection, choose a path
      bool intersection = false;

      direction_e path_options[DIR_COUNT] = {DIR_COUNT};
      int path_option_count = 0;

      direction_e chosen_dir = DIR_COUNT;

      bool found_parking = false;

      for (unsigned int j = 0; j < DIR_COUNT; j++) {
        entity_t *entity = around[j];

        if (entity == NULL) {
          continue;
        }

        if (entity->type == PARKING) {
          print_car(car, false);
          printf("found a parking lot.\n");
          found_parking = true;
          chosen_dir = j;
          break;
        }

        // found an empty road leading from this
        if (entity->type == EMPTY_ROAD) {
          path_options[path_option_count] = j;
          path_option_count += 1;

          printf("found a path option\n");
          print_pos(add_dir(car->pos, j));

          // more than one, we're on an intersection
          if (path_option_count > 1) {
            intersection = true;
            printf("found an intersection\n");
          }
          continue;
        }
      }

      free(around);
      around = NULL;

      // we're supposed to choose a path on the intersection
      // go through the directions, pick randomly (even distr for now)
      if (found_parking) {
        car->speed = set_dir(car->speed, chosen_dir);
      } else {
        if (intersection) {
          direction_e last = DIR_COUNT;

          for (int j = 0; j < path_option_count; j++) {
            direction_e option = path_options[j];

            if (option == DIR_COUNT) {
              continue;
            }

            // don't take the same path back
            if (cmp_pos(add_dir(car->pos, option), car->p_pos)) {
              continue;
            }

            last = option;

            if ((rand() % path_option_count) > 1) {
              chosen_dir = option;
              break;
            }
          }

          // failed to choose, use the last possible option
          if (chosen_dir == DIR_COUNT) {
            chosen_dir = last;
          }
        } else {
          // use the only path option
          for (int j = 0; j < path_option_count; j++) {
            direction_e option = path_options[j];

            if (option != DIR_COUNT) {
              chosen_dir = option;
              break;
            }
          }
        }

        if (chosen_dir == DIR_COUNT) {
          print_car(car, false);
          printf("failed to choose direction\n");
          car->speed = (position_t){0, 0};
        } else {
          print_car(car, false);
          printf("chose path to ");
          print_pos(add_dir(car->pos, chosen_dir));

          car->speed = set_dir(car->speed, chosen_dir);
        }
      }

      move_car(list, car);
    }
  }

  return !cars_parked;
}

int main() {
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

  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    printf("--- Tick #%d\n", tick++);

    bool should_quit = !run(&data);
    draw(renderer, &data);

    if (quit || should_quit) {
      printf("Stopping...\n");
      // Pauza pro zobrazení výsledků
      SDL_Delay(1000);
      quit = true;
      continue;
    }

    if (!quit) {
      // Zpoždění pro lepší pozorování
      SDL_Delay(400);
      printf("---\n");
    }
  }

  free_entity_list(data.entities);
  free_road_list(data.roads);

  close_SDL(window, renderer);
  return 0;
}
