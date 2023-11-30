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

void render_road(SDL_Renderer *renderer, cord_t x, cord_t y) {
  SDL_Rect rect = {.x = x + CELL_SIZE / 2 - ROAD_INDICATOR_WIDTH / 2,
                   .y = y + PADDING / 2,
                   .w = ROAD_INDICATOR_WIDTH,
                   .h = ROAD_INDICATOR_HEIGHT};

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(renderer, &rect);
}

void draw(SDL_Renderer *renderer, entity_list_t *entities) {
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
  for (unsigned int i = 0; i < entities->size; i++) {
    entity_t *entity = entities->entities[i];

    bool padding = true;
    SDL_Color color;

    switch (entity->type) {
      case EMPTY_ROAD: {
        color = ROAD_COLOR;
        padding = false;
        break;
      }
      case CAR: {
        car_t *car = (car_t*)entity;
        color = car->parked ? CAR_PARKED_COLOR : CAR_COLOR;
        padding = true;
        break;
      }
      default: {
        color = UNKNOWN_COLOR;
        break;
      }
    }

    if (entity->c_override) {
      color = entity->color;
    }

    SDL_SetRenderDrawColor(renderer, RGBA(color));

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

  SDL_RenderPresent(renderer);
}
