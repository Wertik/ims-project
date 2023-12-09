#include "entity.h"

entity_list_t *create_entity_list() {
  entity_list_t *list = (entity_list_t *)malloc(sizeof(entity_list_t));
  MERROR(list);

  list->entities = NULL;
  list->size = 0;

  return list;
}

void add_entity(entity_list_t *list, entity_t *entity) {
  list->size += 1;
  list->entities =
      (entity_t **)realloc(list->entities, sizeof(entity_t) * list->size);
  MERROR(list->entities);

  list->entities[list->size - 1] = entity;
}

void rem_entity(entity_list_t *list, entity_t *entity) {
  int index = -1;
  entity_t *found = NULL;

  for (unsigned int i = 0; i < list->size; i++) {
    entity_t *e = list->entities[i];

    if (e == entity) {
      index = i;
      found = e;
      break;
    }
  }

  if (found != NULL && index != -1) {
    // move everything left by one
    for (unsigned int i = index; i + 1 < list->size; i++) {
      list->entities[i] = list->entities[i + 1];
    }

    list->size -= 1;

    list->entities =
        (entity_t **)realloc(list->entities, sizeof(entity_t *) * list->size);
  }
}

entity_t *get_entity(entity_list_t *list, position_t pos) {
  for (unsigned int i = 0; i < list->size; i++) {
    entity_t *entity = list->entities[i];

    if (entity->pos.x == pos.x && entity->pos.y == pos.y) {
      return entity;
    }
  }
  return NULL;
}

entity_t **get_surroundings(entity_list_t *list, position_t pos) {
  entity_t **res = (entity_t **)malloc(sizeof(entity_t *) * DIR_COUNT);
  MERROR(res);

  memset(res, 0, sizeof(entity_t *) * DIR_COUNT);

  for (unsigned int i = 0; i < list->size; i++) {
    entity_t *e = list->entities[i];

    // is 1 above the position?
    if (e->pos.x == pos.x) {
      if (e->pos.y == pos.y - 1) {
        res[DIR_UP] = e;
        continue;
      } else if (e->pos.y == pos.y + 1) {
        res[DIR_DOWN] = e;
        continue;
      }
    }

    if (e->pos.y == pos.y) {
      if (e->pos.x == pos.x - 1) {
        res[DIR_LEFT] = e;
        continue;
      } else if (e->pos.x == pos.x + 1) {
        res[DIR_RIGHT] = e;
        continue;
      }
    }
  }
  return res;
}

void del_entity(entity_list_t *list, unsigned int idx) {
  if (idx >= list->size) {
    fprintf(stderr, "del_entity: invalid index %d\n", idx);
    exit(EXIT_FAILURE);
  }

  entity_t *entity = list->entities[idx];

  for (unsigned int i = idx; i + 1 < list->size; i++) {
    list->entities[i] = list->entities[i + 1];
  }
  list->size -= 1;

  free_entity(entity);
}

void free_entity_list(entity_list_t *list) {
  for (unsigned int i = 0; i < list->size; i++) {
    entity_t *e = list->entities[i];
    free_entity(e);
  }
  free(list->entities);
  free(list);
}

entity_t *create_entity(position_t pos) {
  entity_t *entity = (entity_t *)malloc(sizeof(entity_t));
  MERROR(entity);

  entity->pos = pos;
  entity->c_override = false;

  return entity;
}

e_road_t *create_road_entity(position_t pos) {
  e_road_t *road = (e_road_t *)malloc(sizeof(e_road_t));
  MERROR(road);

  road->pos = pos;
  road->type = EMPTY_ROAD;
  road->direction = DIR_COUNT;
  road->c_override = false;

  return road;
}

void free_road_entity(e_road_t *e) { free(e); }

const char *pretty_type(entity_type_e type) {
  switch (type) {
    case EMPTY_ROAD:
      return "EMPTY_ROAD";
    case PARKING:
      return "PARKING";
    case MAP_EXIT:
      return "MAP_EXIT";
    default:
      return "UNKNOWN_TYPE";
  }
  return "UNKNOWN_TYPE";
}

void print_entity(entity_t *e, bool nl) {
  switch (e->type) {
    case EMPTY_ROAD:
      print_road_e((e_road_t *)e, nl);
      break;
    default:
      VERBOSE("entity %s @ [%d;%d]", pretty_type(e->type), e->pos.x, e->pos.y);
      if (nl) {
        printf("\n");
      }
      break;
  }
}

void print_road_e(e_road_t *road, bool nl) {
  VERBOSE("empty road @ [%d;%d]", road->pos.x, road->pos.y);

  if (nl == true) {
    VERBOSE("\n");
  } else {
    VERBOSE(": ");
  }
}

void free_entity(entity_t *entity) {
  if (entity == NULL) {
    return;
  }

  switch (entity->type) {
    case EMPTY_ROAD:
      free_road_entity((e_road_t *)entity);
      break;
    default:
      free(entity);
      break;
  }
}

void create_entities(entity_list_t *list, position_t positions[], int count,
                     creator_fn_t creator_fn) {
  for (int i = 0; i < count; i++) {
    position_t pos = positions[i];

    entity_t *e = creator_fn(pos);

    add_entity(list, e);
  }
}

entity_t *creator_road(position_t pos) {
  e_road_t *road = create_road_entity(pos);
  road->direction = DIR_COUNT;
  return (entity_t *)road;
}

entity_t *creator_parking(position_t pos) {
  entity_t *entity = create_entity(pos);
  entity->type = PARKING;
  return entity;
}
