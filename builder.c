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
  // -- build roads

  build_road(data, (position_t[]){{9, 10}, {10, 10}, {11, 10}, {12, 10}}, 4,
             DIR_RIGHT);

  build_road(data, (position_t[]){{13, 11}, {13, 12}, {13, 13}}, 3, DIR_DOWN);

  build_road(data, (position_t[]){{13, 9}, {13, 8}, {13, 7}}, 3, DIR_UP);

  // -- build intersections

  build_intersection(data,
                     // parts
                     (position_t[]){{13, 10}}, 1,
                     // wait spots
                     (inter_spot_data_t[]){{.pos = {12, 10}, .dir = DIR_LEFT}},
                     1);

  // -- add parking spots

  create_entities(data->entities, (position_t[]){{13, 14}, {13, 6}}, 2,
                  creator_parking);

  // -- add cars

  create_entities(data->entities, (position_t[]){{9, 10}}, 1, creator_car);
}
