#include "simulation.h"

void run_generators(simulation_data_t *data) {
  for (int i = 0; i < data->generators->size; i++) {
    generator_t *gen = data->generators->data[i];
    run_generator(data, gen);
  }
}

void run_generator(simulation_data_t *data, generator_t *gen) {
  if (data->tick % gen->interval == 0 &&
      (gen->gen_count < gen->count || gen->count == -1)) {
    if (get_car(data->cars, gen->pos) != NULL) {
      printf("cannot generate on [%d;%d], spot occupied.\n", gen->pos.x,
             gen->pos.y);
      return;
    }

    gen->gen_count += 1;

    car_t *car = create_car(gen->pos);
    car->spawned_at = data->tick;
    add_car(data->cars, car);

    printf("Generated a car on [%d;%d] at %d\n", car->pos.x, car->pos.y,
           data->tick);
  }
}

void run_inters(simulation_data_t *data) {
  inter_list_t *inters = data->intersections;

  for (int i = 0; i < inters->size; i++) {
    inter_t *inter = inters->data[i];

    run_inter(data, inter);
  }
}

// intersection simulation logic
// if there are cars waiting on the intersection, signal one of them to proceed
void run_inter(simulation_data_t *data, inter_t *inter) {
  // ignore unused
  data = data;

  if (inter->wait_count == 0 || inter->occupied) {
    return;
  }

  // single car, allow to go
  if (inter->wait_count == 1) {
    for (direction_e dir = DIR_UP; dir < DIR_COUNT; dir++) {
      car_t *car = inter->wait_cars[dir];

      if (car == NULL) {
        continue;
      }

      car->waiting = false;
      inter->occupied = true;
      rem_car_wait_spot(inter, dir);
      break;
    }
    return;
  }

  car_t *car;
  car_t *right_car;
  car_t *opposite_car;

  // Detection of vehicles waiting for priority
  for (direction_e dir = DIR_UP; dir < DIR_COUNT; dir++) {
    car = inter->wait_cars[dir];

    if (car == NULL) {
      continue;
    }

    right_car = inter->wait_cars[(dir + 1 + DIR_COUNT) % DIR_COUNT];
    opposite_car = inter->wait_cars[(dir + 2) % DIR_COUNT];  // Car opposite

    printf("Car %d position: %d, %d\n", dir, car->pos.x, car->pos.y);

    if (right_car != NULL) {
      car->waiting = true;
      printf("car %d is waiting for car %d to go\n", dir,
             (dir + 1 + DIR_COUNT) % DIR_COUNT);
    } else if (opposite_car != NULL && (car->pos.x < opposite_car->pos.x ||
                                        car->pos.y < opposite_car->pos.y)) {
      car->waiting = true;
      printf("car %d is waiting for the opposite car to go\n", dir);
    } else {
      printf("car %d is not waiting\n", dir);
      car->waiting = false;
    }
  }

  // Assigning priority to uninterrupted vehicles
  for (direction_e dir = DIR_UP; dir < DIR_COUNT; dir++) {
    car = inter->wait_cars[dir];
    right_car = inter->wait_cars[(dir + 1 + DIR_COUNT) % DIR_COUNT];
    opposite_car = inter->wait_cars[(dir + 2) % DIR_COUNT];  // Car opposite

    if (car == NULL || car->waiting) {
      continue;
    }

    // Cars that are not in a waiting state
    inter->occupied = true;
    rem_car_wait_spot(inter, dir);
    printf("car picked direction %d to go\n", dir);
    car->waiting = false;
  }
}

bool run_cars(simulation_data_t *data) {
  bool cars_left = true;

  car_list_t *list = data->cars;

  for (int i = 0; i < list->size;) {
    car_t *car = list->data[i];

    printf("- ");
    print_car(car, true);

    // no need to do anything when the car already left
    if (car->left == true) {
      cars_left &= cars_left;
      i++;
      continue;
    }
    cars_left = false;

    // car simulation logic

    // path finding logic
    // intersections, roads, etc.
    // modifies speed
    run_car(data, car);

    // apply speed to the current position of the car
    move_car(data, car);

    // the car left, repeat this index (entities got shifted left)
    if (car->left == true) {
      continue;
    }
    i++;
  }

  // detect cars on the same spot
  for (cord_t x = 0; x < GRID_WIDTH; x++) {
    for (cord_t y = 0; y < GRID_HEIGHT; y++) {
      bool found = false;

      for (int i = 0; i < data->cars->size; i++) {
        car_t *car = data->cars->data[i];

        if (car->pos.x == x && car->pos.y == y) {
          if (found == true) {
            data->paused = true;
            printf("Multiple cars on the same spot (%d;%d). Pausing...\n", x,
                   y);
            // break out of all the loops
            goto end;
          } else {
            found = true;
          }
        }
      }
    }
  }

end:
  return !cars_left;
}

void run_car(simulation_data_t *data, car_t *car) {
  if (car->parked) {
    // once we waited for long enough, head towards the exit
    if (data->tick >= car->parked_at + CAR_PARKED_TICKS) {
      car->leaving = true;
      car->parked = false;
      printf("leaving the parking lot.\n");
    }
    return;
  }

  // waiting on intersection, don't move
  if (car->waiting) {
    car->speed = (position_t){.x = 0, .y = 0};
    return;
  }

  // car has specific directions to take
  // move by those
  if (car->nav_count != 0) {
    direction_e dir = get_nav_step(car);

    position_t target = add_dir(car->pos, dir);

    // target position occupied
    if (get_car(data->cars, target) != NULL) {
      print_car(car, false);
      car->speed = (position_t){0, 0};
      printf("cannot follow steps - would lead to collision, waiting.\n");
      return;
    }

    // direction before leaving the intersection, un-occupy the intersection
    if (car->nav_count == 1) {
      inter_t *inter = get_inter(data->intersections, car->pos);

      if (inter != NULL) {
        printf("set intersection to un-occupied\n");
        inter->occupied = false;
      }
    }

    pop_nav_step(car);
    car->speed = set_dir(car->speed, dir);
    print_car(car, false);
    printf("following steps\n");
    return;
  }

  // "path finding"

  // possible paths around the car
  entity_t **around = get_surroundings(data->entities, car->pos);

  direction_e path_options[DIR_COUNT] = {DIR_COUNT};
  int path_option_count = 0;

  direction_e chosen_dir = DIR_COUNT;

  for (direction_e dir = 0; dir < DIR_COUNT; dir++) {
    entity_t *entity = around[dir];

    if (entity == NULL) {
      continue;
    }

    // position occupied by another car, don't consider this path
    if (get_car(data->cars, entity->pos) != NULL) {
      continue;
    }

    // prio parking lots
    // - only head into it if it's empty
    if (entity->type == PARKING) {
      // if we're already leaving, we're not interested in parking spots
      // if it's already occupied, don't consider this spot
      if (car->leaving == true) {
        continue;
      }

      print_car(car, false);
      printf("found a parking lot.\n");
      chosen_dir = dir;
      break;
    }

    if (entity->type == MAP_EXIT) {
      chosen_dir = dir;
      printf("found exit.\n");
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

      printf("found path option ");
      print_pos(entity->pos);
    }
    continue;
  }

  // not decided yet, pick from the options
  if (chosen_dir == DIR_COUNT) {
    // go through the directions, pick randomly (even distr for now)
    if (path_option_count > 1) {
      // should be current piece of road
      entity_t *curr_e = get_entity(data->entities, car->pos);
      e_road_t *curr_road = (e_road_t *)curr_e;

      if (curr_e->type == EMPTY_ROAD && curr_road->direction != DIR_COUNT) {
        chosen_dir = curr_road->direction;
      } else {
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

          if ((rand() % (path_option_count + 1)) > 1) {
            chosen_dir = option;
            break;
          }
        }

        // failed to choose, use the last possible option
        if (chosen_dir == DIR_COUNT) {
          chosen_dir = last;
        }
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
  }

  if (chosen_dir == DIR_COUNT) {
    print_car(car, false);
    printf("failed to choose direction\n");
    car->speed = (position_t){0, 0};

    free(around);
    return;
  }

  print_car(car, false);
  printf("chose path to ");
  print_pos(add_dir(car->pos, chosen_dir));

  car->speed = set_dir(car->speed, chosen_dir);

  // -- moving to an intersection wait point

  e_road_t *wait_spot = (e_road_t *)around[chosen_dir];

  // if the road is an waiting point for intersection
  inter_t *inter = get_inter_wait(data->intersections, wait_spot->pos);

  if (inter != NULL) {
    // add the car to waiting cars on the intersection
    direction_e inter_dir = get_inter_wait_dir(inter, wait_spot->pos);

    car->waiting = true;

    add_car_wait_spot(inter, inter_dir, car);
    printf("waiting on intersection, coming from %d\n", inter_dir);

    int count = 0;
    for (direction_e dir = DIR_UP; dir < DIR_COUNT; dir++) {
      e_road_t *opt = inter->options[dir];

      if (opt != NULL) {
        count++;
      }
    }

    if (count == 0) {
      printf("no intersection exit to chose from\n");
      exit(EXIT_FAILURE);
    }

    direction_e dir = DIR_UP;

    // decide where we're going on the intersection
    while (true) {
      direction_e curr_dir = dir;

      if ((++dir) >= DIR_COUNT) {
        dir = DIR_UP;
      }

      // don't allow to exit on the same spot we're waiting on
      if (inter_dir == curr_dir) {
        continue;
      }

      e_road_t *opt = inter->options[curr_dir];

      if (opt == NULL) {
        continue;
      }

      // pick this road?
      // - if the road leads to exit and we're leaving
      // - based on chance otherwise

      road_t *road = get_road(data->roads, opt->pos);

      bool choose_road =
          road != NULL && road->has_exit == true && car->leaving == true;

      if (choose_road == false) {
        choose_road = (rand() % (count + 1)) >= 1;
      }

      if (choose_road == true) {
        // navigate the car through the intersection

        int nav_count = 0;
        direction_e *nav =
            get_nav(wait_spot->pos, opt->pos,
                    inter_dir == DIR_DOWN || inter_dir == DIR_UP, &nav_count);

        printf("navigate from ");
        print_pos(wait_spot->pos);
        printf(" to ");
        print_pos(opt->pos);

        if (nav == NULL) {
          printf("run_car: invalid navigation through intersection.\n");
          exit(EXIT_FAILURE);
        }

        // add steps based on the direction
        add_nav_steps(car, nav, nav_count);

        printf("got navigation steps\n");

        // print steps
        for (int i = 0; i < nav_count; i++) {
          printf("- %d\n", nav[i]);
        }

        free(nav);
        break;
      }
    }
  }
  free(around);
}

// move the car according to it's speed
void move_car(simulation_data_t *data, car_t *car) {
  // act on speed, update the car position

  cord_t new_x = bounds(car->pos.x + car->speed.x, 0, GRID_WIDTH - 1);
  cord_t new_y = bounds(car->pos.y + car->speed.y, 0, GRID_HEIGHT - 1);

  if (new_x == car->pos.x && new_y == car->pos.y) {
    return;
  }

  // car can only move to it's destinated cell, if there is an empty road

  entity_t *dst = get_entity(data->entities, (position_t){new_x, new_y});

  // nothing there, cannot move to void
  if (dst == NULL) {
    print_car(car, false);
    printf("cannot move forward, no road ahead.\n");
    return;
  }

  if (dst->type == EMPTY_ROAD) {
    car->p_pos = car->pos;

    car->pos.x = new_x;
    car->pos.y = new_y;

    print_car(car, false);
    printf("moved forward.\n");
    return;
  }

  if (dst->type == MAP_EXIT) {
    car->left = true;

    car->p_pos = car->pos;

    car->pos.x = new_x;
    car->pos.y = new_y;

    car->speed.x = 0;
    car->speed.y = 0;

    print_car(car, false);
    printf("left the map.\n");

    rem_car(data->cars, car);
    free_car(car);
    return;
  }

  if (dst->type == PARKING) {
    car->parked = true;
    car->parked_at = data->tick;

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