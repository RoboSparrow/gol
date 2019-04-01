#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gol.h"
#include "utils.h"
#include "state.h"
#include "pattern.h"
#include "renderer.h"

extern SDL_Renderer *renderer;
extern TTF_Font *font;

extern RendererInfo rendererInfo;
extern SdlColors Colors;

SDL_Rect rect = {
    .x = 0,
    .y = 0,
    .w = UNIT_SIZE,
    .h = UNIT_SIZE
};

SDL_Texture *hints = NULL;

/**
 * render background
 * @param cols world data matrix width
 * @param rows world data matrix height
 */
void render_world_background(int cols, int rows) {
    int ws = rendererInfo.w / cols;
    int hs = rendererInfo.h / rows;

    int i;
    for (i = 0; i < cols; i++) {
        SDL_RenderDrawLine(renderer, ws * i, 0, ws * i, rendererInfo.h);
    }
    SDL_RenderDrawLine(renderer, rendererInfo.w, 0, rendererInfo.w, rendererInfo.h);

    for (i = 0; i < rows; i++) {
        SDL_RenderDrawLine(renderer, 0, hs * i, rendererInfo.w, hs * i);
    }
    SDL_RenderDrawLine(renderer, 0, rendererInfo.h, rendererInfo.w, rendererInfo.h);
}

////
// menu
////

/**
 * initializes hints
 */
void init_world_hints(char text[]) {
    SDL_Surface *surface;

    if (!(surface = TTF_RenderUTF8_Solid(font, text, Colors.text))) {
        LOG_ERROR_F("Failed to render text: %p.", TTF_GetError());
        return;
    }

    hints = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

/**
 * render text hints
 */
void render_world_hints() {
    if (hints == NULL) {
        return;
    }

    int w = 0;
    int h = 0;
    SDL_Rect r = { 0, 0, rendererInfo.w, rendererInfo.h };

    SDL_QueryTexture(hints, NULL, NULL, &w, &h);
    r.w = w;
    r.h = h;
    r.x = (rendererInfo.w - r.w) / 2;
    r.y = rendererInfo.h - r.h - 10;
    SDL_RenderCopy(renderer, hints, NULL, &r);
}

////
// API
////

/**
 * callback for rendering a single cell within an update loop
 * @param cell cell status (alive, dead)
 * @param index cell position in world data matrix
 * @param cols world data matrix width
 * @param rows world data matrix height
 */
void render_world_loop_cell(char cell, int index, int cols, int rows) {
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
 * initialize this screen
 * @param world world pattern
 */
void screen_world_init(Pattern *world) {
    init_world_hints("press enter for restart, space for pause");
}

/**
 * render this screen
 * @param world world pattern
 */
void screen_world_render(Pattern *world) {
    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);

    renderer_set_color(Colors.grid);
    render_world_background(world->cols, world->rows);

    renderer_set_color(Colors.fg);
    gol_update(world, &render_world_loop_cell);

    renderer_set_color(Colors.text);
    render_world_hints();

    SDL_RenderPresent(renderer);
}

/**
 * manages events for this screen
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void screen_world_events(SDL_Event e, GlobalState *App, Pattern *world) {}

/**
 * clear world screen
 */
void screen_world_clear() {
    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

/**
 * destroys this screen and components
 */
void screen_world_destroy() {
      SDL_DestroyTexture(hints);
}

