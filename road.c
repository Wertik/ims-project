#include "road.h"

road_t* create_road() {
  road_t* road = (road_t*)malloc(sizeof(road_t));
  MERROR(road);

  road->parts = NULL;
  road->has_exit = false;
  road->part_count = 0;

  return road;
}

bool is_part(road_t* road, e_road_t* part) {
  for (int i = 0; i < road->part_count; i++) {
    if (road->parts[i] == part) {
      return true;
    }
  }
  return false;
}

void add_part(road_t* road, e_road_t* part) {
  road->parts = (e_road_t**)realloc(road->parts,
                                    sizeof(e_road_t*) * (road->part_count + 1));
  MERROR(road->parts);

  road->parts[road->part_count] = part;
  road->part_count += 1;
}

void free_road(road_t* road) {
  free(road->parts);
  free(road);
}

road_list_t* create_road_list() {
  road_list_t* list = (road_list_t*)malloc(sizeof(road_list_t));
  MERROR(list);

  list->roads = NULL;
  list->size = 0;
  return list;
}

void add_road(road_list_t* list, road_t* road) {
  list->roads =
      (road_t**)realloc(list->roads, sizeof(road_t*) * (list->size + 1));
  MERROR(list->roads);

  list->roads[list->size] = road;
  list->size += 1;
}

void free_road_list(road_list_t* list) {
  for (int i = 0; i < list->size; i++) {
    free_road(list->roads[i]);
  }

  free(list->roads);
  free(list);
}

road_t* get_road(road_list_t* list, position_t pos) {
  for (int i = 0; i < list->size; i++) {
    road_t* road = list->roads[i];

    for (int j = 0; j < road->part_count; j++) {
      entity_t* e = (entity_t*)road->parts[j];

      if (cmp_pos(e->pos, pos) == true) {
        return road;
      }
    }
  }
  return NULL;
}
