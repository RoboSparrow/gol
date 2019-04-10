/**
 * definitions for app state
 */

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "state.h" //todo remove dependency
#include "pattern.h" //todo remove dependency

typedef enum {
    WSTATE_NONE,                    // hidden
    WSTATE_DEFAULT,                 // default appearance
    WSTATE_HOVER,                   // hover, highlighted appearance
    WSTATE_ACTIVE,                  // pressed, highlighted appearance
    WSTATE_RELEASED,                // released, default appearance
} WidgetState;

typedef enum {
    WTYPE_TEXTBOX,
    // entries below may have highlighted appearnces
    WTYPE_BUTTON,
} WidgetType;

typedef struct Widget {
    WidgetType type;                // required
    char *text;                     // required
    SDL_Color *fgcol;               // required: default foreground (text) color, default: rgb(0,0,0)
    SDL_Color *bgcol;               // required: default background color, transparent if NULL
    SDL_Color *hbgcol;              // optional: highlight bg color, default: NULL
    SDL_Rect *rect;                 // required: widget bounding box (x, y, w, h)
    SDL_Texture *texture;           // required: default texture
    SDL_Texture *htexture;          // optional: highlight texture, default NULL
    WidgetState state;              // required: initial state, is updated by SDL events
    void (*action)(SDL_Event e);    // optinal: action callback to be fired, default: NULL
} Widget;


Widget *widget_new(WidgetType type, char *text, SDL_Color *fgcol, SDL_Color *bgcol, SDL_Color *hbgcol);
int widget_build(Widget *widget, SDL_Renderer *ren, TTF_Font *font, int xmargin, int ymargin, int wrap);
void widget_setPostion(Widget *widget, int x, int y);
void widget_render(Widget *widget, SDL_Renderer *ren, SDL_Rect *clip);
void widget_event(Widget *widget, SDL_Event e);
void widget_destroy(Widget *widget);

// utils

int widget_is_in_rect(int x, int y, SDL_Rect *rect);
void widget_print_widget(Widget *widget);
#endif
