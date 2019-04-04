#include <SDL2/SDL.h>

#include "widgets.h"

/**
 * checks if coordinates are in the bounds of an SDL_Rect
 * @return 1 if coordinates are in bounds, otherwise 0
 */
int widgets_is_in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && x < rect->x + rect->w && y >= rect->y && y < rect->y + rect->h;
}

/**
 * print debug info for a widget
 */
void widgets_print_widgets(Widget *widget) {
    printf("%s \n", (strlen(widget->text)) ? widget->text : "<NOTEXT>");
    printf("    |_ [state] %d\n", widget->state);
    printf("    |_ [rect] x: %d, y: %d, w: %d, h: %d,\n", widget->rect.x, widget->rect.y, widget->rect.w, widget->rect.h);
    printf("    |_ [text color] rgba(%d, %d, %d, %d)\n", widget->text_color.r, widget->text_color.g, widget->text_color.b, widget->text_color.a);
    printf("    |_ [bg color] rgba(%d, %d, %d, %d)\n", widget->bg_color.r, widget->bg_color.g, widget->bg_color.b, widget->bg_color.a);
    printf("    |_ [default texture] %p\n", (void *) widget->default_texture);
    printf("    |_ [hover texture] %p\n", (void *) widget->hover_texture);
}
