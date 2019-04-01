/**
 * definitions for app state
 */

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <SDL2/SDL.h>
#include "state.h" //todo remove dependency
#include "pattern.h" //todo remove dependency

typedef enum {
    WSTATE_NONE,
    WSTATE_DEFAULT,
    WSTATE_HOVER,
    WSTATE_ACTIVE,
    WSTATE_MAX,
} WidgetState;

// typedef enum {
//     WIDGET_BUTTON,
//     WIDGET_NONE,
// } WidgetType;

typedef struct Widget {
    //WidgetType type;
    WidgetState state;
    char *text;
    int x;
    int y;
    SDL_Color text_color;
    SDL_Color bg_color;
    SDL_Rect rect;
    SDL_Texture *default_texture;
    SDL_Texture *hover_texture;
} Widget;

int widgets_is_in_rect(int x, int y, SDL_Rect *rect);

Widget *widgets_button_new(char *text);
void widgets_button_init(Widget *btn, SDL_Renderer *ren, TTF_Font *font, int x, int y);
void widgets_button_render(Widget *btn, SDL_Renderer *ren);
void widgets_btn_event(Widget *btn, SDL_Event e);
void widgets_button_destroy(Widget *btn);

#endif
