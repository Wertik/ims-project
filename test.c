#include "car.h"
#include "entity.h"
#include "road.h"

#define ASSERT_EQ(REAL, EXPECTED)                     \
  do {                                                \
    if ((REAL) != (EXPECTED)) {                       \
      VERBOSE("Assert L#%d didn't pass.\n", __LINE__); \
      exit(EXIT_FAILURE);                             \
    }                                                 \
  } while (0);

#define ASSERT_NEQ(REAL, EXPECTED)                    \
  do {                                                \
    if ((REAL) == (EXPECTED)) {                       \
      VERBOSE("Assert L#%d didn't pass.\n", __LINE__); \
      exit(EXIT_FAILURE);                             \
    }                                                 \
  } while (0);

int main() {
  {
    entity_list_t *list = create_entity_list();
    create_entities(list, (position_t[]){{1, 1}, {1, 0}, {2, 1}}, 3,
                    creator_road);

    ASSERT_EQ(list->size, 3);

    entity_t **around = get_surroundings(list, (position_t){1, 1});
    ASSERT_NEQ(around, NULL);

    for (unsigned int dir = DIR_UP; dir < DIR_COUNT; dir++) {
      entity_t *e = around[dir];

      if (dir == DIR_RIGHT || dir == DIR_UP) {
        ASSERT_NEQ(e, NULL);
      }
    }

    free_entity_list(list);
    free(around);
  }

  {
    road_t *road = create_road(DIR_RIGHT);
    ASSERT_NEQ(road, NULL);

    e_road_t *road_e = create_road_entity((position_t){0, 0});
    ASSERT_NEQ(road_e, NULL);

    add_part(road, road_e);

    ASSERT_EQ(road->part_count, 1);
    ASSERT_EQ(road->parts[0], road_e);

    bool res = is_part(road, road_e);
    ASSERT_EQ(res, true);

    free_road_entity(road_e);
    free_road(road);
  }

  {
    car_t *car = create_car((position_t){0, 0});

    add_nav_steps(car, (direction_e[]){DIR_UP, DIR_DOWN}, 2);
    ASSERT_EQ(car->nav_count, 2);

    direction_e first = pop_nav_step(car);
    ASSERT_EQ(first, DIR_UP);
    ASSERT_EQ(car->nav_count, 1);

    direction_e second = pop_nav_step(car);
    ASSERT_EQ(second, DIR_DOWN);
    ASSERT_EQ(car->nav_count, 0);
    ASSERT_EQ(car->nav, NULL);
  }

  {
    int nav_count = 0;
    direction_e *nav =
        get_nav((position_t){0, 0}, (position_t){1, 2}, false, &nav_count);

    ASSERT_NEQ(nav, NULL);
    ASSERT_EQ(nav_count, 3);

    ASSERT_EQ(nav[0], DIR_RIGHT);

    ASSERT_EQ(nav[1], DIR_DOWN);
    ASSERT_EQ(nav[2], DIR_DOWN);
  }

  {
    int nav_count = 0;
    direction_e *nav =
        get_nav((position_t){2, 2}, (position_t){0, 0}, false, &nav_count);

    ASSERT_NEQ(nav, NULL);
    ASSERT_EQ(nav_count, 4);

    ASSERT_EQ(nav[0], DIR_LEFT);
    ASSERT_EQ(nav[1], DIR_LEFT);

    ASSERT_EQ(nav[2], DIR_UP);
    ASSERT_EQ(nav[3], DIR_UP);
  }

  {
    car_list_t *list = create_car_list();

    car_t *car = create_car((position_t){0, 0});
    car_t *car_2 = create_car((position_t){1, 1});

    add_car(list, car);
    add_car(list, car_2);

    rem_car(list, car);

    ASSERT_EQ(list->size, 1);

    free_car_list(list);
  }

  VERBOSE("Tests ran successfully.\n");
  return EXIT_SUCCESS;
}