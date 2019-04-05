/**
 * definitions for app state
 */

#ifndef __STATE_H__
#define __STATE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define UNIT_SIZE 10
#define APP_NAME "RoboSparrow's game of Life"
#define FONT "assets/NotoSans-Regular.ttf"

typedef enum {
    SDL_SCREEN_START,
    SDL_SCREEN_WORLD
} SdlScreen;

typedef enum {
    APP_STATE_QUIT,
    APP_STATE_ONCE,
    APP_STATE_RUNNING,
} AppState;

typedef struct GlobalState {
   AppState state;
   SdlScreen screen;
} GlobalState;

typedef struct SdlColors {
   SDL_Color bg;
   SDL_Color fg;
   SDL_Color grid;
   SDL_Color contrast;
   SDL_Color borders;
   SDL_Color text;
} SdlColors;

typedef struct SdlFonts {
    TTF_Font *body;
} SdlFonts;

extern SdlColors Colors;
extern SdlFonts Fonts;
#endif
