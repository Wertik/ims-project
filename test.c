#include "entity.h"
#include "road.h"

#define ASSERT_EQ(REAL, EXPECTED)                     \
  do {                                                \
    if ((REAL) != (EXPECTED)) {                       \
      printf("Assert L#%d didn't pass.\n", __LINE__); \
      exit(EXIT_FAILURE);                             \
    }                                                 \
  } while (0);

#define ASSERT_NEQ(REAL, EXPECTED)                    \
  do {                                                \
    if ((REAL) == (EXPECTED)) {                       \
      printf("Assert L#%d didn't pass.\n", __LINE__); \
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

  printf("Tests ran successfully.\n");
  return EXIT_SUCCESS;
}