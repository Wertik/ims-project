#include "builder.h"

void build_road(simulation_data_t *data, position_t positions[], int count,
                direction_e dir) {
  road_t *road = create_road();

  for (int i = 0; i < count; i++) {
    position_t pos = positions[i];

    e_road_t *road_e = create_road_entity(pos);
    road_e->direction = dir;

    add_entity(data->entities, (entity_t *)road_e);
    add_part(road, road_e);
  }

  add_road(data->roads, road);
}

void build_intersection(simulation_data_t *data, position_t parts[],
                        int part_count, inter_spot_data_t wait_spots[],
                        int wait_count) {
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

  add_inter(data->intersections, inter);
}

void build_map(simulation_data_t *data) {
  map_e map = SINGLE_INTER;

  switch (map) {
    case SINGLE_INTER:
      // multiple cars meet on a single intersection

      // -- build roads

      build_road(data, (position_t[]){{9, 10}, {10, 10}, {11, 10}}, 3,
                 DIR_RIGHT);

      build_road(data, (position_t[]){{12, 11}, {12, 12}, {12, 13}}, 3, DIR_UP);

      build_road(data, (position_t[]){{12, 9}, {12, 8}, {12, 7}}, 3, DIR_UP);

      build_road(data, (position_t[]){{13, 10}, {14, 10}, {15, 10}}, 3,
                 DIR_LEFT);

      // -- build intersections

      build_intersection(
          data,
          // parts
          (position_t[]){{12, 10}}, 1,
          // wait spots
          (inter_spot_data_t[]){{.pos = {11, 10}, .dir = DIR_LEFT},
                                {.pos = {12, 11}, .dir = DIR_DOWN}},
          2);

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
                 7, DIR_RIGHT);

      // shouldn't be chosen
      build_road(data, (position_t[]){{10, 9}, {10, 8}}, 2, DIR_DOWN);

      // shouldn't be chosen
      build_road(data, (position_t[]){{12, 11}, {12, 12}}, 2, DIR_UP);

      // allowed (but has only a chance)
      build_road(data, (position_t[]){{14, 9}, {14, 8}}, 2, DIR_UP);

      // a turn
      build_road(data, (position_t[]){{16, 10}, {16, 11}}, 2, DIR_DOWN);

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
