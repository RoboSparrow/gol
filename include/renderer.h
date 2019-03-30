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

//global

void renderer_init(Pattern *world);
void renderer_exit();

// utility functions

void renderer_set_color(SDL_Color color);

#endif
