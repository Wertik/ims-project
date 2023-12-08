#include "generator.h"

generator_t *create_gen(position_t pos, int interval, int count) {
  generator_t *gen = (generator_t *)malloc(sizeof(generator_t));
  MERROR(gen);

  gen->pos = pos;
  gen->interval = interval;

  gen->count = count;
  gen->gen_count = 0;

  return gen;
}

generator_t *free_gen(generator_t *gen) { free(gen); }

gen_list_t *create_gen_list() {
  gen_list_t *list = (gen_list_t *)malloc(sizeof(gen_list_t));
  MERROR(list);

  list->data = NULL;
  list->size = 0;

  return list;
}

void add_generator(gen_list_t *list, generator_t *gen) {
  list->data = (generator_t **)realloc(
      list->data, sizeof(generator_t *) * (list->size + 1));
  MERROR(list->data);

  list->data[list->size] = gen;
  list->size += 1;
}

void free_gen_list(gen_list_t *list) {
  for (int i = 0; i < list->size; i++) {
    free_gen(list->data[i]);
  }
  free(list);
}