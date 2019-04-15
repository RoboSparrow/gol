/**
 * definitions for app state
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "pattern.h"
#include "state.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;

typedef struct _RendererInfo {
   int w;
   int h;
} RendererInfo;

//extern RendererInfo rendererInfo;

////
//global
////

void renderer_init(Pattern *world);
void renderer_update();
void renderer_destroy();

////
// start screen
////

void screen_start_init(Pattern *world);
void screen_start_render();
void screen_start_events(SDL_Event e, Pattern *world);
void screen_start_destroy();

////
// world screen
////

void screen_world_init(Pattern *world);
void screen_world_render(Pattern *world);
void screen_world_events(SDL_Event e, Pattern *world);
void screen_world_clear();
void screen_world_destroy();

void render_pattern_static(Pattern *pattern, SDL_Rect board, SDL_Color fgbox, SDL_Color bgbox, SDL_Color cellcol);
////
// utility functions
////

void renderer_set_color(SDL_Color color);
SDL_Color renderer_get_color();
#endif
