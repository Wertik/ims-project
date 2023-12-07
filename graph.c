#include "graph.h"

void initialize_SDL(SDL_Window **window, SDL_Renderer **renderer) {
  SDL_Init(SDL_INIT_VIDEO);

  *window = SDL_CreateWindow("CA Traffic Simulation", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, GRID_WIDTH * CELL_SIZE,
                             GRID_HEIGHT * CELL_SIZE, SDL_WINDOW_SHOWN);

  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
}

void close_SDL(SDL_Window *window, SDL_Renderer *renderer) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void render_dir_indicator(SDL_Renderer *renderer, cord_t pos_x, cord_t pos_y,
                          direction_e dir, bool padding) {
  int x, y, w, h;

  switch (dir) {
    case DIR_UP: {
      x = pos_x * CELL_SIZE + CELL_SIZE / 2 - ROAD_INDICATOR_WIDTH / 2;
      y = pos_y * CELL_SIZE + (padding ? PADDING / 2 : 0);
      w = ROAD_INDICATOR_WIDTH;
      h = ROAD_INDICATOR_HEIGHT;
      break;
    }
    case DIR_DOWN: {
      x = pos_x * CELL_SIZE + CELL_SIZE / 2 - ROAD_INDICATOR_WIDTH / 2;
      y = pos_y * CELL_SIZE - (padding ? PADDING / 2 : 0) + CELL_SIZE -
          ROAD_INDICATOR_WIDTH;
      w = ROAD_INDICATOR_WIDTH;
      h = ROAD_INDICATOR_HEIGHT;
      break;
    }
    case DIR_LEFT: {
      x = pos_x * CELL_SIZE + (padding ? PADDING / 2 : 0);
      y = pos_y * CELL_SIZE + CELL_SIZE / 2 - ROAD_INDICATOR_WIDTH / 2;
      w = ROAD_INDICATOR_HEIGHT;
      h = ROAD_INDICATOR_WIDTH;
      break;
    }
    case DIR_RIGHT: {
      x = pos_x * CELL_SIZE + CELL_SIZE - (padding ? PADDING / 2 : 0) -
          ROAD_INDICATOR_HEIGHT;
      y = pos_y * CELL_SIZE + CELL_SIZE / 2 - ROAD_INDICATOR_WIDTH / 2;
      w = ROAD_INDICATOR_HEIGHT;
      h = ROAD_INDICATOR_WIDTH;
      break;
    }
    default: {
      // no direction set
      return;
    }
  }

  SDL_Rect rect = {x, y, w, h};

  SDL_SetRenderDrawColor(renderer, RGBA(INDICATOR_COLOR));
  SDL_RenderFillRect(renderer, &rect);
}

void render_entity(SDL_Renderer *renderer, entity_t *entity, SDL_Color color,
                   bool padding) {
  SDL_Color real_color = entity->c_override ? entity->color : color;
  SDL_SetRenderDrawColor(renderer, RGBA(real_color));

  SDL_Rect rect = {.x = entity->pos.x * CELL_SIZE,
                   .y = entity->pos.y * CELL_SIZE,
                   .w = CELL_SIZE,
                   .h = CELL_SIZE};

  if (padding) {
    rect.x = rect.x + PADDING / 2;
    rect.y = rect.y + PADDING / 2;
    rect.w = rect.w - PADDING;
    rect.h = rect.h - PADDING;
  }

  SDL_RenderFillRect(renderer, &rect);
}

void draw(SDL_Renderer *renderer, simulation_data_t *data) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  // draw all cell edges
  for (cord_t x = 0; x < GRID_WIDTH; x++) {
    for (cord_t y = 0; y < GRID_HEIGHT; y++) {
      // render edges
      SDL_Rect edge_rect = {.x = x * CELL_SIZE,
                            .y = y * CELL_SIZE,
                            .w = CELL_SIZE,
                            .h = CELL_SIZE};

      SDL_Color color = EDGE_COLOR;
      SDL_SetRenderDrawColor(renderer, RGBA(color));
      SDL_RenderDrawRect(renderer, &edge_rect);
    }
  }

  // render entities
  for (unsigned int i = 0; i < data->entities->size; i++) {
    entity_t *entity = data->entities->entities[i];

    switch (entity->type) {
      case EMPTY_ROAD: {
        e_road_t *road = (e_road_t *)entity;

        render_entity(renderer, entity, ROAD_COLOR, false);

        // if there is a direction on the road,
        // draw an indicator of this direction
        if (road->direction != DIR_COUNT) {
          render_dir_indicator(renderer, road->pos.x, road->pos.y,
                               road->direction, false);
        }
        break;
      }
      case CAR: {
        car_t *car = (car_t *)entity;

        SDL_Color car_color = CAR_COLOR;
        if (car->parked == true) {
          car_color = CAR_PARKED_COLOR;
        } else if (car->leaving == true) {
          car_color = CAR_LEAVING_COLOR;
        }

        render_entity(renderer, entity, car_color, true);
        break;
      }
      case MAP_EXIT: {
        render_entity(renderer, entity, MAP_EXIT_COLOR, false);
        break;
      }
      default: {
        render_entity(renderer, entity, UNKNOWN_COLOR, true);
        break;
      }
    }
  }

  // render road borders

  for (int i = 0; i < data->roads->size; i++) {
    road_t *road = data->roads->roads[i];

    for (int j = 0; j < road->part_count; j++) {
      e_road_t *e = road->parts[j];

      SDL_SetRenderDrawColor(renderer, RGBA(ROAD_EDGE_COLOR));

      SDL_Rect rect = {.x = e->pos.x * CELL_SIZE,
                       .y = e->pos.y * CELL_SIZE,
                       .w = CELL_SIZE,
                       .h = CELL_SIZE};

      SDL_RenderDrawRect(renderer, &rect);
    }
  }

  // render intersection borders
  // and wait spots

  for (int i = 0; i < data->intersections->size; i++) {
    inter_t *inter = data->intersections->data[i];

    for (int j = 0; j < inter->part_count; j++) {
      e_road_t *e = inter->parts[j];

      SDL_SetRenderDrawColor(renderer, RGBA(INTERSECTION_EDGE_COLOR));

      SDL_Rect rect = {.x = e->pos.x * CELL_SIZE,
                       .y = e->pos.y * CELL_SIZE,
                       .w = CELL_SIZE,
                       .h = CELL_SIZE};

      SDL_RenderDrawRect(renderer, &rect);
    }

    // wait spots
    for (direction_e dir = DIR_UP; dir < DIR_COUNT; dir++) {
      e_road_t *e = inter->wait_spots[dir];

      if (e == NULL) {
        continue;
      }

      SDL_SetRenderDrawColor(renderer, RGBA(INTERSECTION_SPOT_EDGE_COLOR));

      SDL_Rect rect = {.x = e->pos.x * CELL_SIZE,
                       .y = e->pos.y * CELL_SIZE,
                       .w = CELL_SIZE,
                       .h = CELL_SIZE};

      SDL_RenderDrawRect(renderer, &rect);
    }

    // intersection exits
    for (direction_e dir = DIR_UP; dir < DIR_COUNT; dir++) {
      e_road_t *e = inter->options[dir];

      if (e == NULL) {
        continue;
      }

      SDL_SetRenderDrawColor(renderer, RGBA(INTERSECTION_EXIT_EDGE_COLOR));

      SDL_Rect rect = {.x = e->pos.x * CELL_SIZE,
                       .y = e->pos.y * CELL_SIZE,
                       .w = CELL_SIZE,
                       .h = CELL_SIZE};

      SDL_RenderDrawRect(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
}
