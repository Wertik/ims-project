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
    entity_t *e = list->entities[i];
    switch (e->type) {
      case EMPTY_ROAD:
        free_road_entity((e_road_t *)e);
        break;
      case CAR:
        free_car((car_t *)e);
        break;
      default:
        free_entity(e);
        break;
    }
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

car_t *create_car(position_t pos) {
  car_t *car = (car_t *)malloc(sizeof(car_t));
  MERROR(car);

  car->type = CAR;
  car->pos = pos;
  car->p_pos = pos;

  car->parked = false;
  car->waiting = false;
  car->intersection = false;

  car->speed = (position_t){.x = 0, .y = 0};
  car->c_override = false;

  car->nav = NULL;
  car->nav_count = 0;

  return car;
}

void free_car(car_t *car) { free(car); }

void print_car(car_t *car, bool nl) {
  if (nl) {
    printf("car @ [%d;%d]: speed=[%d;%d], parked=%s, waiting=%s\n", car->pos.x,
           car->pos.y, car->speed.x, car->speed.y, car->parked ? "yes" : "no",
           car->waiting ? "yes" : "no");
  } else {
    printf("car @ [%d;%d]: speed=[%d;%d], parked=%s, waiting=%s: ", car->pos.x,
           car->pos.y, car->speed.x, car->speed.y, car->parked ? "yes" : "no",
           car->waiting ? "yes" : "no");
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

entity_t *creator_car(position_t pos) {
  car_t *car = create_car(pos);
  return (entity_t *)car;
}

void add_nav_steps(car_t *car, direction_e steps[], int count) {
  int new_count = car->nav_count + count;

  car->nav =
      (direction_e *)realloc(car->nav, sizeof(direction_e) * (new_count));
  MERROR(car->nav);

  for (int i = car->nav_count; i < new_count; i++) {
    car->nav[i] = steps[i - car->nav_count];
  }

  car->nav_count = new_count;
}

// pop a navigation step
direction_e pop_nav_step(car_t *car) {
  if (car->nav == NULL) {
    return DIR_COUNT;
  }

  direction_e res = car->nav[0];

  if (car->nav_count == 1) {
    // last element
    free(car->nav);
    car->nav = NULL;
    car->nav_count = 0;
    return res;
  }

  // shift all steps left by one
  for (int i = 0; i + 1 < car->nav_count; i++) {
    car->nav[i] = car->nav[i + 1];
  }

  // shrink by one
  car->nav = (direction_e *)realloc(
      car->nav, sizeof(direction_e *) * (car->nav_count - 1));
  MERROR(car->nav);

  car->nav_count -= 1;
  return res;
}
