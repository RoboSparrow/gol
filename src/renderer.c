#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "state.h"
#include "pattern.h"
#include "utils.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font *font = NULL;

////
// global functions
////

/**
 * initializes Sdl and Sdl modules
 */
void renderer_init(Pattern *world) {
    // SDL core

    int sizeX = UNIT_SIZE * world->cols;
    int sizeY = UNIT_SIZE * world->rows;

    int init = SDL_Init(SDL_INIT_EVERYTHING);
    EXIT_MINUS_F(init, "Failed to initialize SDL: %s.", SDL_GetError());

    window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
    EXIT_NULL_F(window, "Failed to create SDL window: %s.", SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, 0);
    EXIT_NULL_F(renderer, "Failed to create SDL renderer: %s.", SDL_GetError());

    // SDL ttf

    Path path;
    path_build(FONT, path);
    int font_size = 10;

    int tinit = TTF_Init();
    EXIT_MINUS_F(tinit, "Failed to initialize SDL_TTF: %s.", TTF_GetError());

    font = TTF_OpenFont(path, font_size);
    EXIT_NULL_F(font, "Failed to load SDL_TTF font: %s.", TTF_GetError());

    SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
}

/**
 * exits Sdl and Sdl modules
 */
void renderer_exit() {
    if(font) {
        TTF_CloseFont(font);
        font = NULL;
    }

    if( renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if(window){
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_Quit();
}

////
// utility functions
////

/**
 * set current renderer drawing color
 */
void renderer_set_color(SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}
