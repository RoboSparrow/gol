#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gol.h"
#include "utils.h"
#include "state.h"
#include "pattern.h"
#include "renderer.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;

extern SdlColors Colors;

SDL_Rect rect = {
    .x = 0,
    .y = 0,
    .w = UNIT_SIZE,
    .h = UNIT_SIZE
};

// renderer logical output size
// todo to sdl.h SDL_RenderGetLogicalSize
int rw = 0;
int rh = 0;

/**
 * render background
 */
void render_world_background(int cols, int rows) {
    int ws = rw / cols;
    int hs = rh / rows;

    int i;
    for (i = 0; i < cols; i++) {
        SDL_RenderDrawLine(renderer, ws * i, 0, ws * i, rh);
    }
    SDL_RenderDrawLine(renderer, rw, 0, rw, rh);

    for (i = 0; i < rows; i++) {
        SDL_RenderDrawLine(renderer, 0, hs * i, rw, hs * i);
    }
    SDL_RenderDrawLine(renderer, 0, rh, rw, rh);
}

////
// menu
////

/**
 * render text hints
 */
void render_world_hints(char text[]) {
    int w = 0;
    int h = 0;
    SDL_Rect text_rect = { 0, 0, rw, rh };
    SDL_Surface *surface;
    SDL_Texture *texture;

    if (!(surface = TTF_RenderUTF8_Solid(font, text, Colors.text))) {
        LOG_ERROR_F("Failed to render text: %p.", TTF_GetError());
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    text_rect.w = w;
    text_rect.h = h;
    text_rect.x = (rw - text_rect.w) / 2;
    text_rect.y = rh - text_rect.h - 10;

    SDL_RenderCopy(renderer, texture, NULL, &text_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

////
// loop
////

/**
 * callback for rendering a single cell within an update loop
 */
void render_world_loop_cell(int cell, int index, int cols, int rows) {
    // SDL_Log("cell index %d, status %d", index, cell);
    if (cell == GOL_ALIVE) {

        int row = gol_row(index, cols);
        int col = gol_col(index, row, cols);

        rect.x = UNIT_SIZE * col;
        rect.y = UNIT_SIZE * row;

        SDL_RenderFillRect(renderer, &rect);
    }
}

////
// api
////

/**
 * render world screen
 */
void render_world(Pattern *world) {

    SDL_RenderGetLogicalSize(renderer, &rw, &rh);

    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);

    renderer_set_color(Colors.grid);
    render_world_background(world->cols, world->rows);

    renderer_set_color(Colors.fg);
    gol_update(world, &render_world_loop_cell);

    renderer_set_color(Colors.text);
    render_world_hints("press enter for restart, space for pause");//TODO daw only once and cache

    SDL_RenderPresent(renderer);
    //render_world_loop_end(world->cols, world->rows);
}

/**
 * clear world screen
 */
void render_clear_world() {
    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

