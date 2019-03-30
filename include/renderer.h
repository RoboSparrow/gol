/**
 * definitions for app state
 */

#ifndef _RENDERER_H__
#define __RENDERER_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "pattern.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font *font;

////
//global
////

void renderer_init(Pattern *world);
void renderer_destroy();

////
// start screen
////

void render_start(Pattern *world);

////
// world screen
////

void screen_world_init();
void screen_world_render(Pattern *world);
void screen_world_clear();
void screen_world_destroy();

////
// utility functions
////

void renderer_set_color(SDL_Color color);

#endif
