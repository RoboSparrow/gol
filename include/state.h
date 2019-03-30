/**
 * definitions for app state
 */

#ifndef __STATE_H__
#define __STATE_H__

#include <SDL2/SDL.h>

#define UNIT_SIZE 10
#define APP_NAME "RoboSparrow's game of Life"
#define FONT "assets/hellovetica.ttf"

typedef enum {
    SDL_SCREEN_START,
    SDL_SCREEN_WORLD
} SdlScreen;

typedef struct SdlColors {
   SDL_Color bg;
   SDL_Color fg;
   SDL_Color grid;
   SDL_Color text;
} SdlColors;

extern SdlColors Colors;
#endif
