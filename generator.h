#pragma once

#include "pos.h"
#include "util.h"

typedef struct {
  // interval of this generator
  int interval;
  // how many cars to generate
  int count;

  // how many cars were already generated
  int gen_count;

  // where to generate cars
  position_t pos;
} generator_t;

typedef struct {
  int size;
  generator_t **data;
} gen_list_t;

generator_t *create_gen(position_t pos, int interval, int count);
void free_gen(generator_t *gen);

gen_list_t *create_gen_list();
void add_generator(gen_list_t *list, generator_t *gen);
void free_gen_list(gen_list_t *list);
