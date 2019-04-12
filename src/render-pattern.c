#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#include "state.h"
#include "pattern.h"
#include "renderer.h"
#include "gol.h"

extern SDL_Renderer *renderer;
extern SdlColors Colors;

/**
 * Render a static pattern
 */
void render_pattern_static(Pattern *pattern, SDL_Rect canvas, SDL_Color fgcol, SDL_Color bgcol, SDL_Color bordercol) {
    int unitSize = 0;

    // zoom
    int dimX = ceil(canvas.w / pattern->cols);
    int dimY = ceil(canvas.h / pattern->rows);
    unitSize = (dimX < dimY) ? dimX : dimY;

    if (unitSize < 2) {
        unitSize = 2;
    }

    if (unitSize > 5) {
        unitSize = 5;
    }

    int offsetX = (canvas.w - (pattern->cols * unitSize)) / 2;
    int offsetY = (canvas.h - (pattern->rows * unitSize)) / 2;

    SDL_Rect unit = {
        .x = 0,
        .y = 0,
        .w = unitSize,
        .h = unitSize
    };

    SDL_Color cached = renderer_get_color();
    int size = pattern->cols * pattern->rows;
    int col = 0;
    int row = 0;
    char *data = pattern->data;

    renderer_set_color(bgcol);
    SDL_RenderFillRect(renderer, &canvas);
    renderer_set_color(fgcol);

    for (int i = 0; i < size; i++) {
        if(data[i] == GOL_ALIVE) {
            unit.x = offsetX + canvas.x + (col * unitSize);
            unit.y = offsetY + canvas.y + (row * unitSize);
            if (unit.x) {
                SDL_RenderFillRect(renderer, &unit);
            }
        }

        if((i + 1) % pattern->cols == 0) {
            col = 0;
            row++;
            continue;
        }
        col++;
    }
    renderer_set_color(bordercol);
    SDL_RenderDrawRect(renderer, &canvas);

    renderer_set_color(cached);
}
