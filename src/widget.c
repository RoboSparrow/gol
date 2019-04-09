#include <SDL2/SDL.h>

#include "widget.h"

int widget_is_in_rect(int x, int y, SDL_Rect *rect);

/**
 * Creates a a rendering rect from widgte bounding box dimensions and an optional clipping rect
 * @param src widget bounding box rect
 * @param clipping rect
 * @return rect to be rendered
 */
static SDL_Rect adjusted_clip_rect(SDL_Rect *src, SDL_Rect *clip) {
    SDL_Rect rect = { src->x, src->y, src->w, src->h };

    if (clip != NULL) {
        rect.x = src->x + clip->x;
        rect.y = src->y + clip->y;
        rect.w = clip->w;
        rect.h = clip->h;
    }

    return rect;
}

/**
 * Optionally render witdget decorations on render
 */
static void widget_render_decoration(Widget *widget, SDL_Renderer *ren, SDL_Rect *clip) {
    RETURN_VOID_IF(widget == NULL || widget->bgcol == NULL || widget->rect == NULL);
    // TODO
}

/**
 * create a background surface (to be blitted with foreground)
 * @param bgrect widget dimension,  only "w" and "h" properties are considered
 * @param bgcol widget background color. Pass NULL if bg is transparent
 * @return surface or null on failure
 */
static SDL_Surface *create_background_surface(SDL_Rect *bgrect, SDL_Color *bgcol) {
    SDL_Surface* temp;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    temp = SDL_CreateRGBSurface(0, bgrect->w, bgrect->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    #else
    temp = SDL_CreateRGBSurface(0, bgrect->w, bgrect->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    #endif
    if(bgcol != NULL) {
        Uint32 ubgcol = SDL_MapRGB(temp->format, bgcol->r,bgcol->b, bgcol->g);
        SDL_FillRect(temp, NULL, ubgcol);
    }
    // SDL_BLENDMODE_BLEND is actually the default blend mode
    SDL_SetSurfaceBlendMode(temp, SDL_BLENDMODE_BLEND);
    return temp;
}

/**
* Draws an SDL_Texture to an SDL_Renderer to some destination rect, optinally clipping the rendered box
* @param ren The renderer we want to draw to
* @param texture The source texture we want to draw
* @param rect The destination rectangle to render the texture to
* @param clip If != NULL, the sub-section of the texture to draw (clipping rect)
*/
static void widget_render_texture(SDL_Renderer *ren, SDL_Texture *texture, SDL_Rect *rect, SDL_Rect *clip) {
    RETURN_VOID_IF(texture == NULL);
    SDL_Rect adjusted_rect = adjusted_clip_rect(rect, clip);
    SDL_RenderCopy(ren, texture, clip, &adjusted_rect);
}

////
// API
////

/**
 * Debug print a widget
 */
void widget_print_widget(Widget *widget) {
    RETURN_VOID_IF_ERR(widget == NULL, "Cannot print widget: is NULL");

    printf("%s \n", (strlen(widget->text)) ? widget->text : "<NOTEXT>");
    printf("    |_ [state] %d\n", widget->state);
    printf("    |_ [rect] x: %d, y: %d, w: %d, h: %d,\n", widget->rect->x, widget->rect->y, widget->rect->w, widget->rect->h);
    printf("    |_ [text color] rgba(%d, %d, %d, %d)\n", widget->fgcol->r, widget->fgcol->g, widget->fgcol->b, widget->fgcol->a);
    if (widget->bgcol != NULL) {
        printf("    |_ [bg color] rgba(%d, %d, %d, %d)\n", widget->bgcol->r, widget->bgcol->g, widget->bgcol->b, widget->bgcol->a);
    } else {
        printf("    |_ [bg color] NULL\n");
    }
    if (widget->texture != NULL) {
        printf("    |_ [default texture] %p\n", (void *) widget->texture);
    } else {
        printf("    |_ [default texture] NULL\n");
    }
    if (widget->htexture != NULL) {
        printf("    |_ [highlight texture] %p\n", (void *) widget->htexture);
    } else {
        printf("    |_ [highlight texture] NULL\n");
    }
}

/**
 * Intialize a widget,set properties to NULL or assign values.
 */
Widget *widget_new(WidgetType type, char *text, SDL_Color *fgcol, SDL_Color *bgcol, SDL_Color *hbgcol) {
    size_t tlen = strlen(text);

    Widget *widget = malloc(sizeof(Widget));
    RETURN_VAL_IF_ERR(widget == NULL, NULL, "couldn't allocate memory for Widget");

    widget->type = type;
    widget->text = NULL;
    widget->fgcol = NULL;
    widget->bgcol = NULL;
    widget->hbgcol = NULL;
    widget->rect = NULL;
    widget->action = NULL;
    widget->texture = NULL;
    widget->htexture = NULL;

    widget->text = malloc(tlen);
    RETURN_VAL_IF_ERR(widget->text == NULL, NULL, "couldn't allocate memory for Widget text");
    strcpy(widget->text, text);

    // foreground color
    widget->fgcol = malloc(sizeof(SDL_Color));
    RETURN_VAL_IF_ERR(widget->fgcol == NULL, NULL, "couldn't allocate memory for Widget fg color");
    if (fgcol == NULL) {
        fprintf(stderr, "No foreground color set for widgdet!\n");
        widget->fgcol->r = 0;
        widget->fgcol->g = 0;
        widget->fgcol->b = 0;
    } else {
        memcpy(widget->fgcol, fgcol, sizeof(SDL_Color));
    }

    // background color
    if (bgcol != NULL) {
        widget->bgcol = malloc(sizeof(SDL_Color));
        RETURN_VAL_IF_ERR(widget->bgcol == NULL, NULL, "couldn't allocate memory for Widget bg color");
        memcpy(widget->bgcol, bgcol, sizeof(SDL_Color));
    }

    // !! if solid type (no hover), return here
    if(widget->type < WTYPE_BUTTON) {
        return widget;
    }

    // hover background color
    if (hbgcol != NULL) {
        widget->hbgcol = malloc(sizeof(SDL_Color));
        RETURN_VAL_IF_ERR(widget->hbgcol == NULL, NULL, "couldn't allocate memory for Widget hightlight bg color");
        memcpy(widget->hbgcol, hbgcol, sizeof(SDL_Color));
    }

    return widget;

}

/**
 * Deallocate widget and properties, destroy SDL textures
 */
void widget_destroy( Widget *widget ) {
    if (widget == NULL) {
        return;
    }
    if (widget->text != NULL) {
        free(widget->text);
    }
    if (widget->fgcol != NULL) {
        free(widget->fgcol);
    }
    if (widget->bgcol != NULL) {
        free(widget->bgcol);
    }
    if (widget->hbgcol != NULL) {
        free(widget->hbgcol);
    }
    if (widget->rect != NULL) {
        free(widget->rect);
    }
    if (widget->texture != NULL) {
        SDL_DestroyTexture(widget->texture);
    }
    if (widget->htexture != NULL) {
        SDL_DestroyTexture(widget->htexture);
    }
    widget = NULL;
}

/**
 * Build a widget textures and cache them in widget struct.
 * Do this before the entering the render loop
 * @return -1 on failure, otherwise 0
 */
int widget_build(Widget *widget, SDL_Renderer *ren, TTF_Font *font, int x, int y) {
    RETURN_VAL_IF(widget == NULL, -1);

    int margin = 5;

    //flags
    int blitted;
    SDL_Surface *fgsurf = NULL;
    SDL_Surface *bgsurf = NULL;
    SDL_Surface *hbgsurf = NULL;

    // text surface
    switch(widget->type) {
        case WTYPE_TEXTBOX:
            fgsurf = TTF_RenderText_Blended_Wrapped(font, widget->text, *(widget->fgcol), 200);
        break;
        default:
            fgsurf = TTF_RenderText_Blended(font, widget->text, *(widget->fgcol));
    }
    RETURN_VAL_IF_ERR(fgsurf == NULL, -1, "Failed to render text for widget!");

    // box geometry
    SDL_Rect textclip = { margin , margin, fgsurf->w, fgsurf->h };
    SDL_Rect rect = {  x + margin, y + margin, fgsurf->w + (2 * margin), fgsurf->h + (2 * margin) };
    widget->rect = malloc(sizeof(SDL_Rect));
    RETURN_VAL_IF_ERR(widget->rect == NULL, -1, "couldn't allocate memory for Widget rect");
    memcpy(widget->rect, &rect, sizeof(SDL_Rect));

    // background texture
    bgsurf = create_background_surface(widget->rect, widget->bgcol);
        RETURN_VAL_IF_ERR(bgsurf == NULL, -1, "couldn't allocate memory for Widget texture (default)");
    blitted = SDL_BlitSurface(fgsurf, NULL, bgsurf, &textclip);
        RETURN_VAL_IF_ERR(blitted < 0, -1, "couldn't blit surfaces for Widget texture (default)");
    widget->texture = malloc(sizeof(SDL_Texture*));
        RETURN_VAL_IF_ERR(widget->texture == NULL, -1, "couldn't allocate memory for Widget texture (default)");
    widget->texture = SDL_CreateTextureFromSurface(ren, bgsurf);
        RETURN_VAL_IF_ERR(widget->texture == NULL, -1, "couldn't create default texture (default)");

    // highlight texture
    if(widget->hbgcol) {
        hbgsurf = create_background_surface(widget->rect, widget->hbgcol);
            RETURN_VAL_IF_ERR(hbgsurf == NULL, -1, "couldn't allocate memory for Widget texture (highlight)");
        blitted = SDL_BlitSurface(fgsurf, NULL, hbgsurf, &textclip);
            RETURN_VAL_IF_ERR(blitted < 0, -1, "couldn't blit surfaces for Widget texture (highlight)");
        widget->htexture = malloc(sizeof(SDL_Texture*));
            RETURN_VAL_IF_ERR(widget->texture == NULL, -1, "couldn't allocate memory for Widget texture (highlight)");
        widget->htexture = SDL_CreateTextureFromSurface(ren, hbgsurf);
            RETURN_VAL_IF_ERR(widget->texture == NULL, -1, "couldn't create default texture (highlight)");
    }

    // It is safe to pass NULL to SDL_FreeSurface.
    SDL_FreeSurface(hbgsurf);
    SDL_FreeSurface(bgsurf);
    SDL_FreeSurface(fgsurf);

    return 0;
}

/**
 * Render a widget.
 * Befpore rendering the widget needs to be;
 *  1. initialized
 *  2. built
 */
void widget_render(Widget *widget, SDL_Renderer *ren, SDL_Rect *clip) {
    RETURN_VOID_IF(widget == NULL);

    SDL_Texture *selected;
    switch(widget->state) {
        case WSTATE_DEFAULT:
            widget_render_decoration(widget, ren, clip);
            selected = widget->texture;
            break;
        case WSTATE_HOVER:
        case WSTATE_ACTIVE:
        case WSTATE_RELEASED:
            widget_render_decoration(widget, ren, clip);
            selected = (widget->htexture != NULL) ? widget->htexture : widget->texture;
            break;
        case WSTATE_NONE:
        default:
            selected = NULL;
            break;
    }

    widget_render_texture(ren, selected, widget->rect, clip);
}

/**
 * SDL event dispatcher
 */
void widget_event(Widget *widget, SDL_Event e) {
    RETURN_VOID_IF(widget == NULL);

    if(!widget_is_in_rect(e.motion.x, e.motion.y, widget->rect)) {
        widget->state = WSTATE_DEFAULT;
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        widget->state = WSTATE_ACTIVE;
        return;
    }

    // fire W_STATE_RELEASED only once
    if (e.type == SDL_MOUSEBUTTONUP) {
        if(widget->state == WSTATE_ACTIVE) {
            if(widget->action != NULL) {
                widget->action(e);
            }
            widget->state = WSTATE_RELEASED;
            return;
        }
        widget->state = WSTATE_DEFAULT;
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        widget->state = WSTATE_HOVER;
        return;
    }

}

/**
 * mouse detection for widget
 */
int widget_is_in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && x < rect->x + rect->w && y >= rect->y && y < rect->y + rect->h;
}

