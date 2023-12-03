#include "car.h"

bool run_cars(simulation_data_t *data) {
  bool cars_parked = true;

  entity_list_t *list = data->entities;

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

      // path finding logic
      // intersections, roads, etc.
      // modifies speed
      run_car(data, car);

      // apply speed to the current position of the car
      move_car(data->entities, car);
    }
  }

  return !cars_parked;
}

void run_car(simulation_data_t *data, car_t *car) {
  // "path finding"

  // if there is a parking lot on the side of the road, head into it
  entity_t **around = get_surroundings(data->entities, car->pos);

  // otherwise if we are on an intersection, choose a path
  bool intersection = false;

  // path options to choose randomly from
  direction_e path_options[DIR_COUNT] = {DIR_COUNT};
  int path_option_count = 0;

  direction_e chosen_dir = DIR_COUNT;

  bool found_parking = false;

  for (direction_e dir = 0; dir < DIR_COUNT; dir++) {
    entity_t *entity = around[dir];

    if (entity == NULL) {
      continue;
    }

    // head into a parking lot instantly, don't hesitate
    if (entity->type == PARKING) {
      print_car(car, false);
      printf("found a parking lot.\n");
      found_parking = true;
      chosen_dir = dir;
      break;
    }

    // found an empty road leading from this
    // add to options
    if (entity->type == EMPTY_ROAD) {
      // if the road part has a set direction, check for it
      // if the direction is ok for us (aka not directly inverse to the one we
      // wanna try and head in), add to path options

      e_road_t *road = (e_road_t *)entity;

      if (road->direction != DIR_COUNT) {
        if (road->direction == inverse_dir(dir)) {
          // not an option
          printf("path option [%d;%d] not allowed dir.\n", road->pos.x,
                 road->pos.y);
          continue;
        }
      }

      path_options[path_option_count] = dir;
      path_option_count += 1;

      printf("found a path option\n");
      print_pos(add_dir(car->pos, dir));

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

      for (int i = 0; i < path_option_count; i++) {
        direction_e option = path_options[i];

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
}

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
