#include "car.h"

car_t *create_car(position_t pos) {
  car_t *car = (car_t *)malloc(sizeof(car_t));
  MERROR(car);

  car->pos = pos;
  car->p_pos = pos;

  car->parked = false;
  car->waiting = false;
  car->leaving = false;
  car->left = false;

  car->left_at = -1;
  car->parked_at = -1;
  car->spawned_at = 0;

  car->speed = (position_t){.x = 0, .y = 0};
  car->c_override = false;

  car->nav = NULL;
  car->nav_count = 0;

  car->inter_nav = NULL;
  car->inter_nav_count = 0;

  return car;
}

void print_car(car_t *car, bool nl) {
  VERBOSE(
      "car @ [%d;%d]: speed=[%d;%d], parked=%s, waiting=%s, leaving=%s, "
      "left=%s, spawned_at=%d, parked_at=%d",
      car->pos.x, car->pos.y, car->speed.x, car->speed.y, BTS(car->parked),
      BTS(car->waiting), BTS(car->leaving), BTS(car->left), car->spawned_at, car->parked_at);
  if (nl) {
    VERBOSE("\n");
  } else {
    VERBOSE(": ");
  }
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

// get the next navigation step
direction_e get_nav_step(car_t *car) {
  if (car->nav == NULL) {
    return DIR_COUNT;
  }

  return car->nav[0];
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

void free_car(car_t *car) { free(car); }

car_list_t *create_car_list() {
  car_list_t *list = (car_list_t *)malloc(sizeof(car_list_t));
  MERROR(list);

  list->size = 0;
  list->data = NULL;

  return list;
}

void add_car(car_list_t *list, car_t *car) {
  list->data =
      (car_t **)realloc(list->data, sizeof(car_t *) * (list->size + 1));
  MERROR(list->data);

  list->data[list->size] = car;
  list->size += 1;
}

void rem_car(car_list_t *list, car_t *car) {
  int index = -1;
  car_t *found = NULL;

  for (int i = 0; i < list->size; i++) {
    car_t *e = list->data[i];

    if (e == car) {
      index = i;
      found = e;
      break;
    }
  }

  if (found != NULL && index != -1) {
    // move everything left by one
    for (int i = index; i + 1 < list->size; i++) {
      list->data[i] = list->data[i + 1];
    }

    list->size -= 1;

    list->data = (car_t **)realloc(list->data, sizeof(car_t *) * list->size);
  }
}

car_t *get_car(car_list_t *list, position_t pos) {
  for (int i = 0; i < list->size; i++) {
    car_t *car = list->data[i];

    if (car->pos.x == pos.x && car->pos.y == pos.y) {
      return car;
    }
  }
  return NULL;
}

car_t **get_cars_around(car_list_t *list, position_t pos) {
  car_t **res = (car_t **)malloc(sizeof(car_t *) * DIR_COUNT);
  MERROR(res);

  memset(res, 0, sizeof(car_t *) * DIR_COUNT);

  for (int i = 0; i < list->size; i++) {
    car_t *car = list->data[i];

    // is 1 above the position?
    if (car->pos.x == pos.x) {
      if (car->pos.y == pos.y - 1) {
        res[DIR_UP] = car;
        continue;
      } else if (car->pos.y == pos.y + 1) {
        res[DIR_DOWN] = car;
        continue;
      }
    }

    if (car->pos.y == pos.y) {
      if (car->pos.x == pos.x - 1) {
        res[DIR_LEFT] = car;
        continue;
      } else if (car->pos.x == pos.x + 1) {
        res[DIR_RIGHT] = car;
        continue;
      }
    }
  }
  return res;
}

void free_car_list(car_list_t *list) {
  for (int i = 0; i < list->size; i++) {
    free_car(list->data[i]);
  }
  free(list);
}
