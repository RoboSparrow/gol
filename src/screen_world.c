#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gol.h"
#include "utils.h"
#include "state.h"
#include "pattern.h"
#include "renderer.h"

extern SDL_Renderer *renderer;

extern RendererInfo rendererInfo;
extern SdlColors Colors;
extern SdlFonts Fonts;

SDL_Rect unit = {
    .x = 0,
    .y = 0,
    .w = UNIT_SIZE,
    .h = UNIT_SIZE
};

// world
SDL_Rect board = {
    .x = 0,
    .y = 0,
    .w = 0,
    .h = 0,
};

SDL_Texture *hints = NULL;

int paused = 0;
int centered = 1;

static void update_board(int cols, int rows) {

    int mx;
    int my;
    SDL_GetMouseState(&mx, &my);

    int xp = 0;
    int yp = 0;
    int mpadding = 5; // padding for areas around edges who trigger panning
    int bmargin = 20; // margin for board panning

    if (mx < mpadding && board.x < bmargin) {
        centered = 0;
        xp = 1;
    }

    if (mx > rendererInfo.w - mpadding && board.x > -bmargin) {
        centered = 0;
        xp = -1;
    }

    if (my < mpadding && board.y < bmargin) {
        centered = 0;
        yp = 1;
    }

    if (my > rendererInfo.h - mpadding && board.y > -bmargin) {
        centered = 0;
        yp = -1;
    }

    // zoom
    board.w = cols * unit.w;
    board.h = rows * unit.h;

    // pan
    if (centered) {
        board.x = (rendererInfo.w - board.w)/ 2;
        board.y = (rendererInfo.h - board.h)/ 2;
    } else {
        board.x += (xp * unit.w);
        board.y += (yp * unit.h);
    }

}

/**
 * render background
 * @param cols world data matrix width
 * @param rows world data matrix height
 */
static void render_world_background(int cols, int rows) {
    int x0 = board.x;
    int x1 = board.x + board.w;
    int y0 = board.y;
    int y1 = board.y + board.h;

    int xi = unit.w % cols;
    int yi = unit.h % rows;

    int i;
    for (i = 0; i <= cols; i++) {
        // vertical lines
        int x = (xi * i) + x0;
        SDL_RenderDrawLine(renderer, x, y0, x, y1);
    }
    for (i = 0; i <= rows; i++) {
        // horizontal lines
        int y = (yi * i) + y0;
        SDL_RenderDrawLine(renderer, x0, y, x1, y);
    }
}

////
// menu
////

/**
 * initializes hints
 */
static void init_world_hints(char text[]) {
    SDL_Surface *surface;

    if (!(surface = TTF_RenderUTF8_Solid(Fonts.body, text, Colors.text))) {
        LOG_ERROR_F("Failed to render text: %p.", TTF_GetError());
        return;
    }

    hints = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

/**
 * render text hints
 */
static void render_world_hints() {
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
static void render_world_loop_cell(char cell, int index, int cols, int rows) {
    // SDL_Log("cell index %d, status %d", index, cell);
    if (cell == GOL_ALIVE) {
        int row = gol_row(index, cols);
        int col = gol_col(index, row, cols);

        unit.x = board.x + (unit.w * col);
        unit.y = board.y + (unit.w * row);

        SDL_RenderFillRect(renderer, &unit);
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
    update_board(world->cols, world->rows);
    init_world_hints("press enter for restart, space for pause");
}

/**
 * render this screen
 * @param world world pattern
 */
void screen_world_render(Pattern *world) {
    if (paused) {
        return;
    }

    update_board(world->cols, world->rows); //maybe better in events?

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
 * @param world world pattern
 */
void screen_world_events(SDL_Event e, Pattern *world) {
    const int step = 1;

    if (e.type == SDL_MOUSEWHEEL) {
        // todo move calculations to update functions
        if(e.wheel.y == -1) {
            if (unit.w - step > MIN_UNIT_SIZE) {
                unit.w -= step;
                unit.h = unit.w;
            }

        } else if (e.wheel.y == 1) {
            if (unit.w + step < MAX_UNIT_SIZE) {
                unit.w += step;
                unit.h = unit.w;
            }
        }
    }

    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_SPACE:
                paused = !paused;
            break;
            case SDLK_RETURN:
                screen_world_clear();
                paused = 0;
                SDL_Delay(200);
                int started = game_restart(world);
                EXIT_MINUS(started, "Could not restart game\n"); // TODO screen message!
            break;
        }
    }
}

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
