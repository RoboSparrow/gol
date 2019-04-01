#include <SDL2/SDL.h>

int widgets_is_in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && x < rect->x + rect->w && y >= rect->y && y < rect->y + rect->h;
}
