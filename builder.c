#include "builder.h"

#include <unistd.h>

void build_road(simulation_data_t *data, position_t positions[], int count,
                direction_e dir, bool has_exit) {
  road_t *road = create_road();

  for (int i = 0; i < count; i++) {
    position_t pos = positions[i];

    e_road_t *road_e = create_road_entity(pos);
    road_e->direction = dir;

    add_entity(data->entities, (entity_t *)road_e);
    add_part(road, road_e);
  }

  road->has_exit = has_exit;

  add_road(data->roads, road);
}

void build_intersection(simulation_data_t *data, position_t parts[],
                        int part_count, inter_spot_data_t wait_spots[],
                        int wait_count, inter_spot_data_t options[],
                        int option_count) {
  inter_t *inter = create_inter();

  for (int i = 0; i < part_count; i++) {
    position_t pos = parts[i];

    e_road_t *road_e = create_road_entity(pos);
    add_entity(data->entities, (entity_t *)road_e);
    add_inter_part(inter, road_e);
  }

  for (int i = 0; i < wait_count; i++) {
    inter_spot_data_t spot_data = wait_spots[i];

    position_t pos = spot_data.pos;

    e_road_t *road_e = (e_road_t *)get_entity(data->entities, pos);
    add_inter_wait_spot(inter, spot_data.dir, road_e);
  }

  for (int i = 0; i < option_count; i++) {
    e_road_t *road = (e_road_t *)get_entity(data->entities, options[i].pos);
    inter->options[options[i].dir] = road;
  }

  add_inter(data->intersections, inter);
}

void generate_cars(simulation_data_t *data) {
  position_t starting_positions[] = {{5, 3}, {18, 27}};
  int number_of_cars = 1;

  for (int i = 0; i < number_of_cars; ++i) {
    create_entities(data->entities, (position_t[]){starting_positions[i]}, 1,
                    creator_car);
  }
}

void build_map(simulation_data_t *data, map_e map) {
  printf("Building map %d...\n", map);

  switch (map) {
    case LEAVE_NAV: {
      // test that the car always chooses the map exit when leaving

      BUILD_ROAD(data, ARR({{5, 10}, {6, 10}, {7, 10}, {8, 10}, {9, 10}}),
                 DIR_RIGHT);

      BUILD_EXIT_ROAD(data, ARR({{10, 9}, {10, 8}, {10, 7}, {10, 6}}), DIR_UP);

      BUILD_ROAD(data, ARR({{11, 10}, {12, 10}, {13, 10}}), DIR_RIGHT);
      BUILD_ROAD(data, ARR({{14, 10}, {14, 11}, {14, 12}}), DIR_DOWN);
      BUILD_ROAD(data, ARR({{14, 13}, {13, 13}, {12, 13}, {11, 13}}), DIR_LEFT);
      BUILD_ROAD(data, ARR({{10, 13}, {10, 12}, {10, 11}}), DIR_UP);

      BUILD_INTER(data, ARR({{10, 10}}),
                  ARR({{{9, 10}, DIR_LEFT}, {{10, 11}, DIR_DOWN}}),
                  ARR({{{10, 9}, DIR_UP}, {{11, 10}, DIR_RIGHT}}));

      entity_t *map_exit = create_entity((position_t){10, 5});
      map_exit->type = MAP_EXIT;
      add_entity(data->entities, map_exit);

      // spawn a car
      car_t *car = create_car((position_t){5, 10});
      car->leaving = true;
      add_entity(data->entities, (entity_t *)car);
      break;
    }
    case MULTI_INTER:
      // intersection on a 2 lane road

      // -- build roads

      BUILD_ROAD(data, ARR({{9, 9}, {10, 9}, {11, 9}, {12, 9}}), DIR_LEFT);

      BUILD_ROAD(data, ARR({{9, 9}, {10, 9}, {11, 9}, {12, 9}}), DIR_LEFT);

      BUILD_ROAD(data, ARR({{9, 10}, {10, 10}, {11, 10}, {12, 10}}), DIR_RIGHT);
      BUILD_ROAD(data, ARR({{14, 11}, {14, 12}, {14, 13}, {14, 14}}), DIR_UP);
      BUILD_ROAD(data, ARR({{13, 11}, {13, 12}, {13, 13}, {13, 14}}), DIR_DOWN);

      // -- build intersections

      BUILD_INTER(data, ARR({{13, 10}, {14, 10}, {13, 9}, {14, 9}}),
                  ARR({{.pos = {12, 10}, .dir = DIR_LEFT},
                       {.pos = {14, 11}, .dir = DIR_DOWN}}),
                  ARR({{.pos = {12, 9}, .dir = DIR_LEFT},
                       {.pos = {13, 11}, .dir = DIR_DOWN}}));

      // -- add parking spots

      BUILD_PARKING(data, ARR({{8, 9}, {13, 15}}));

      // -- add cars

      ADD_CARS(data, ARR({{9, 10}, {14, 14}}));
      break;
    case PARKING_LOT: {
      // -- build extended vertical roads for car entrances
      position_t road_positions[] = {
          {5, 3},  {5, 4},  {5, 5},  {5, 6},  {5, 7},  {5, 8},  {5, 9},
          {5, 10}, {5, 11}, {5, 12}, {5, 13}, {5, 14}, {5, 15}, {5, 16},
          {5, 17}, {5, 18}, {5, 19}, {5, 21}, {5, 22}, {5, 23}};

      int num_positions = sizeof(road_positions) / sizeof(road_positions[0]);

      build_road(data, road_positions, num_positions, DIR_DOWN, true);

      build_road(data,
                 (position_t[]){{18, 10},
                                {18, 11},
                                {18, 13},
                                {18, 15},
                                {18, 17},
                                {18, 19},
                                {18, 21},
                                {18, 22},
                                {18, 23},
                                {18, 24},
                                {18, 25},
                                {18, 26},
                                {18, 27}},
                 13, DIR_UP, false);

      /// -- build horizontal roads for parking access, all going left except
      /// the bottom one
      for (int row = 0; row < 6; ++row) {
        int y_offset = 10 + (row * 2);
        direction_e dir = (row == 5) ? DIR_RIGHT : DIR_LEFT;
        build_road(data,
                   (position_t[]){{6, y_offset},
                                  {7, y_offset},
                                  {8, y_offset},
                                  {9, y_offset},
                                  {10, y_offset},
                                  {11, y_offset},
                                  {12, y_offset},
                                  {13, y_offset},
                                  {14, y_offset},
                                  {15, y_offset},
                                  {16, y_offset},
                                  {17, y_offset}},
                   12, dir, false);
      }

      // cars starting position

      position_t intersection_positions[] = {
          {18, 12}, {18, 14}, {18, 16}, {18, 18}};
      int num_intersections = 4;

      for (int i = 0; i < num_intersections; i++) {
        position_t inter_pos = intersection_positions[i];

        BUILD_INTER(
            data, ARR({inter_pos}),
            ARR({{.pos = {inter_pos.x, inter_pos.y + 1}, .dir = DIR_DOWN}}),
            ARR({{.pos = {inter_pos.x, inter_pos.y - 1}, .dir = DIR_UP},
                 {.pos = {inter_pos.x - 1, inter_pos.y}, .dir = DIR_LEFT}}));
      }

      BUILD_INTER(data, ARR({{18, 20}}),
                  ARR({{.pos = {18, 21}, .dir = DIR_DOWN},
                       {.pos = {17, 20}, .dir = DIR_LEFT}}),
                  ARR({{.pos = {18, 19}, .dir = DIR_UP}}));

      BUILD_INTER(data, ARR({{5, 20}}), ARR({{{5, 19}, DIR_UP}}),
                  ARR({{{6, 20}, DIR_RIGHT}, {{5, 21}, DIR_DOWN}}));

      // -- add parking spots next to each road
      for (int row = 0; row < 5; ++row) {
        int y_offset = 10 + (row * 2) + 1;
        for (int spot = 0; spot < 12; ++spot) {
          int x_offset = 6 + spot;
          create_entities(data->entities, (position_t[]){{x_offset, y_offset}},
                          1, creator_parking);
        }
      }

      // -- add map exit
      entity_t *map_exit = create_entity((position_t){5, 24});
      map_exit->type = MAP_EXIT;
      add_entity(data->entities, map_exit);

      // -- adjust road direction for the ends of the vertical roads

      e_road_t *top_right =
          (e_road_t *)get_entity(data->entities, (position_t){18, 10});
      top_right->direction = DIR_LEFT;
      break;
    }

    case SINGLE_INTER:
      // multiple cars meet on a single intersection

      // -- build roads

      build_road(data, (position_t[]){{9, 10}, {10, 10}, {11, 10}}, 3,
                 DIR_RIGHT, false);

      build_road(data, (position_t[]){{12, 11}, {12, 12}, {12, 13}}, 3, DIR_UP, false);

      build_road(data, (position_t[]){{12, 9}, {12, 8}, {12, 7}}, 3, DIR_UP, false);

      build_road(data, (position_t[]){{13, 10}, {14, 10}, {15, 10}}, 3,
                 DIR_LEFT, false);

      // -- build intersections

      BUILD_INTER(data, ARR({{12, 10}}),
                  ARR({{{11, 10}, DIR_LEFT},
                       {{12, 11}, DIR_DOWN},
                       {{13, 10}, DIR_RIGHT}}),
                  ARR({{{12, 9}, DIR_UP}}));

      // -- add parking spots

      create_entities(data->entities, (position_t[]){{12, 6}, {13, 7}, {11, 7}},
                      3, creator_parking);

      // -- add cars

      create_entities(data->entities,
                      (position_t[]){{9, 10}, {12, 13}, {15, 10}}, 3,
                      creator_car);
      break;
    case ROAD_DIR:
      // test road option picking for allowed directions on road tiles

      // -- build roads

      build_road(data,
                 (position_t[]){{9, 10},
                                {10, 10},
                                {11, 10},
                                {12, 10},
                                {13, 10},
                                {14, 10},
                                {15, 10}},
                 7, DIR_RIGHT, false);

      // shouldn't be chosen
      build_road(data, (position_t[]){{10, 9}, {10, 8}}, 2, DIR_DOWN, false);

      // shouldn't be chosen
      build_road(data, (position_t[]){{12, 11}, {12, 12}}, 2, DIR_UP, false);

      // allowed (but has only a chance)
      build_road(data, (position_t[]){{14, 9}, {14, 8}}, 2, DIR_UP, false);

      // a turn
      build_road(data, (position_t[]){{16, 10}, {16, 11}}, 2, DIR_DOWN, false);

      // -- add parking spots

      // parking lots at the end to stop the simulation
      create_entities(data->entities, (position_t[]){{16, 12}, {14, 7}}, 2,
                      creator_parking);

      // -- add cars
      create_entities(data->entities, (position_t[]){{9, 10}}, 1, creator_car);
      break;
    default:
      printf("build_map: Invalid map chosen.\n");
      exit(EXIT_FAILURE);
  }
}
