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
  entity_t **res = (entity_t **)malloc(sizeof(entity_t *) * IDX_COUNT);
  MERROR(res);

  bzero(res, sizeof(entity_t *) * IDX_COUNT);

  for (unsigned int i = 0; i < list->size; i++) {
    entity_t *e = list->entities[i];

    // is 1 above the position?
    if (e->pos.x == pos.x) {
      if (e->pos.y == pos.y - 1) {
        res[IDX_UP] = e;
        continue;
      } else if (e->pos.y == pos.y + 1) {
        res[IDX_DOWN] = e;
        continue;
      }
    }

    if (e->pos.y == pos.y) {
      if (e->pos.x == pos.x - 1) {
        res[IDX_LEFT] = e;
        continue;
      } else if (e->pos.x == pos.x + 1) {
        res[IDX_RIGHT] = e;
        continue;
      }
    }
  }
  return res;
}

void del_entity(entity_list_t *list, unsigned int idx) {
  if (idx >= list->size) {
    printf("del_entity: invalid index %d\n", idx);
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
    free_entity(list->entities[i]);
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

car_t *create_car(position_t pos) {
  car_t *car = (car_t *)malloc(sizeof(car_t));
  MERROR(car);

  car->type = CAR;
  car->pos = pos;
  car->p_pos = pos;
  car->parked = false;
  car->speed = (position_t){.x = 0, .y = 0};
  car->c_override = false;

  return car;
}
void free_car(car_t *car) { free(car); }

void print_car(car_t *car, bool nl) {
  if (nl) {
    printf("car @ [%d;%d]: speed=[%d;%d], parked=%s\n", car->pos.x, car->pos.y,
           car->speed.x, car->speed.y, car->parked ? "yes" : "no");
  } else {
    printf("car @ [%d;%d]: speed=[%d;%d], parked=%s: ", car->pos.x, car->pos.y,
           car->speed.x, car->speed.y, car->parked ? "yes" : "no");
  }
}

void free_entity(entity_t *entity) { free(entity); }

void create_entities(entity_list_t *list, position_t positions[], int count,
                     creator_fn_t creator_fn) {

  for (int i = 0; i < count; i++) {
    position_t pos = positions[i];

    entity_t *e = creator_fn(pos);

    add_entity(list, e);
  }
}
