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
void renderer_exit();

////
// start screen
////

void render_start(Pattern *world);

////
// world screen
////

void render_world(Pattern *world);
void render_clear_world();

////
// utility functions
////

void renderer_set_color(SDL_Color color);

#endif
