/**
 * definitions for app state
 */

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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

typedef struct WidgetConfig {
    int id;                         // required
    WidgetType type;                // required
    WidgetState state;              // required: initial state, is updated by SDL events
    char *text;                     // required
    SDL_Color *fgColor;             // required: default fg (text) color, default: rgb(0,0,0)
    SDL_Color *bgColor;             // optional: default bg color, default: NULL
    SDL_Color *borderColor;         // optional: default border color, default: NULL
    SDL_Color *fgColorH;            // required: highlight foreground (text) color, default: rgb(0,0,0)
    SDL_Color *bgColorH;            // optional: highlight bg color, default: NULL
    SDL_Color *borderColorH;        // optional: highlight border color, default: NULL
    void (*action)(SDL_Event e);    // optional: action callback to be fired, default: NULL
    int paddingX;                   // optional: padding bg to text, default: 5
    int paddingY;                   // optional: padding bg to text, default: 5
    int wrap;                       // optional: text wrapping width, default: 0
} WidgetConfig;

typedef struct Widget {
    int id;                         // required
    WidgetType type;                // required
    WidgetState state;              // required: initial state, is updated by SDL events
    SDL_Rect *rect;                 // required: widget bounding box (x, y, w, h)
    SDL_Texture *texture;           // required: default texture
    SDL_Texture *htexture;          // optional: highlight texture, default NULL
    void (*action)(SDL_Event e);    // optinal: action callback to be fired, default: NULL
} Widget;

WidgetConfig widget_configure(int id, WidgetType type, char *text);
Widget *widget_build(WidgetConfig config, SDL_Renderer *ren, TTF_Font *font);
void widget_set_postion(Widget *widget, int x, int y);
void widget_render(Widget *widget, SDL_Renderer *ren, SDL_Rect *clip);
void widget_event(Widget *widget, SDL_Event e);
void widget_destroy(Widget *widget);

// utils

int widget_is_in_rect(int x, int y, SDL_Rect *rect);
void widget_print_widget(Widget *widget);
void widget_print_widgetconfig(WidgetConfig config);
#endif
