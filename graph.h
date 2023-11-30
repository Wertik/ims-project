#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "entity.h"

// grid settings
#define GRID_HEIGHT 32
#define GRID_WIDTH 32

#define GRID_SIZE GRID_WIDTH *GRID_HEIGHT

// real cell size
#define CELL_SIZE 20

// padding between the cell color and the edges
// has to be even
#define PADDING 4

// convert SDL_Color to r, g, b, a for render functions
#define RGBA(C) C.r, C.b, C.b, C.a

// create SDL_Color struct
#define SDL(r, g, b, a) \
  { r, g, b, a }

// -- Cell colors
// rgba

#define EDGE_COLOR \
  (SDL_Color) { 168, 168, 168, 255 }

#define ROAD_COLOR \
  (SDL_Color) { 168, 168, 168, 255 }

#define ROAD_INDICATOR_WIDTH 4
#define ROAD_INDICATOR_HEIGHT 8

#define CAR_COLOR \
  (SDL_Color) { 255, 0, 0, 255 }

#define CAR_PARKED_COLOR \
  (SDL_Color) { 0, 125, 125, 255 }

#define UNKNOWN_COLOR \
  (SDL_Color) { 189, 189, 189, 255 }

void initialize_SDL(SDL_Window **window, SDL_Renderer **renderer);

void close_SDL(SDL_Window *window, SDL_Renderer *renderer);

void draw(SDL_Renderer *renderer, entity_list_t *entities);
