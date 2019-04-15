#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "widget.h"
#include "utils.h"
#include "state.h"
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
    RETURN_VOID_IF(widget == NULL || widget->rect == NULL);
    // TODO
}

/**
 * create a widget texture by blending sufaces for forground (text) and background
 */
// todo free surfaces on error
static SDL_Texture *create_widget_texture(SDL_Renderer *renderer, TTF_Font *font, char *text, WidgetType type, SDL_Color *fgColor, SDL_Color *bgColor, SDL_Color *borderColor, int paddingX, int paddingY, int wrap) {
    SDL_Surface *bgsurf;
    SDL_Surface *fgsurf;
    SDL_Texture* texture;

    if(wrap > 0) {
        fgsurf = TTF_RenderText_Blended_Wrapped(font, text, *(fgColor), wrap);
    } else {
        fgsurf = TTF_RenderText_Blended(font, text, *(fgColor));
    }
    RETURN_VAL_IF_ERR(fgsurf == NULL, NULL, "Failed to render widget text!");

    int w = fgsurf->w + (2 * paddingX);
    int h = fgsurf->h + (2 * paddingY);

    if (bgColor != NULL) {
        bgsurf = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    } else {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        bgsurf = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        #else
        bgsurf = SDL_CreateRGBSurface(0, w, h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        #endif
    }
    RETURN_VAL_IF_ERR(bgsurf == NULL, NULL, "Failed to create surface for widget background!");

    if (bgColor != NULL) {
        SDL_FillRect(bgsurf, NULL, SDL_MapRGB(bgsurf->format, bgColor->r, bgColor->g, bgColor->b));
    }

    if (borderColor != NULL) {
        Uint32 bcol = SDL_MapRGB(bgsurf->format, borderColor->r, borderColor->g, borderColor->b);
        SDL_Rect top = {0, 0, bgsurf->w, 1};
        SDL_Rect left = {0, 0, 1, bgsurf->h};
        SDL_Rect right = {bgsurf->w - 1, 0, 1, bgsurf->h};
        SDL_Rect bottom = {0, bgsurf->h - 1, bgsurf->w, 1};

        SDL_FillRect(bgsurf, &top, bcol);
        SDL_FillRect(bgsurf, &left, bcol);
        SDL_FillRect(bgsurf, &right, bcol);
        SDL_FillRect(bgsurf, &bottom, bcol);
    }

    SDL_Rect fgclip = { paddingX, paddingY, bgsurf->w, bgsurf->h };
    int blitted = SDL_BlitSurface(fgsurf, NULL, bgsurf, &fgclip);
    RETURN_VAL_IF_ERR(blitted < 0, NULL, "Failed to blit surfaces for widget texture!");

    texture = SDL_CreateTextureFromSurface(renderer, bgsurf);
    RETURN_VAL_IF_ERR(texture == NULL, NULL, "Failed to create widget texture! from surface!");

    SDL_FreeSurface(bgsurf);
    SDL_FreeSurface(fgsurf);

    return texture;
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
 * Debug print a widget config instance
 */
void widget_print_widgetconfig(WidgetConfig config) {
    printf("widget config: [%d]\n", config.id);
    printf("    |_ [text] %s\n", (strlen(config.text)) ? config.text : "<NOTEXT>");
    printf("    |_ [type] %d\n", config.type);
    printf("    |_ [state] %d\n", config.state);
    if (config.fgColor != NULL) {
        printf("    |_ [fg color] rgba(%d, %d, %d, %d)\n", config.fgColor->r, config.fgColor->g, config.fgColor->b, config.fgColor->a);
    } else {
        printf("    |_ [fg color] NULL\n");
    }
    if (config.bgColor != NULL) {
        printf("    |_ [bg color] rgba(%d, %d, %d, %d)\n", config.bgColor->r, config.bgColor->g, config.bgColor->b, config.bgColor->a);
    } else {
        printf("    |_ [bg color] NULL\n");
    }
    if (config.borderColor != NULL) {
        printf("    |_ [border color] rgba(%d, %d, %d, %d)\n", config.borderColor->r, config.borderColor->g, config.borderColor->b, config.borderColor->a);
    } else {
        printf("    |_ [border color] NULL\n");
    }
    if (config.fgColorH != NULL) {
        printf("    |_ [highlight fg color] rgba(%d, %d, %d, %d)\n", config.fgColorH->r, config.fgColorH->g, config.fgColorH->b, config.fgColorH->a);
    } else {
        printf("    |_ [highlight fg color] NULL\n");
    }
    if (config.bgColorH != NULL) {
        printf("    |_ [highlight bg color] rgba(%d, %d, %d, %d)\n", config.bgColorH->r, config.bgColorH->g, config.bgColorH->b, config.bgColorH->a);
    } else {
        printf("    |_ [highlight bg color] NULL\n");
    }
    if (config.borderColorH != NULL) {
        printf("    |_ [highlight border color] rgba(%d, %d, %d, %d)\n", config.borderColorH->r, config.borderColorH->g, config.borderColorH->b, config.borderColorH->a);
    } else {
        printf("    |_ [highlight border color] NULL\n");
    }
    if (config.action != NULL) {
        printf("    |_ [action callback] yes\n");
    } else {
        printf("    |_ [action callback] NULL\n");
    }
    printf("    |_ [margin x] %d\n", config.paddingX);
    printf("    |_ [margin y] %d\n", config.paddingY);
    printf("    |_ [wrap] %d\n", config.wrap);
}

/**
 * Debug print a widget instance
 */
void widget_print_widget(Widget *widget) {
    RETURN_VOID_IF_ERR(widget == NULL, "Cannot print widget: is NULL");

    printf("widget [%d]: %p \n", widget->id, (void *)widget);
    printf("    |_ [state] %d\n", widget->state);
    printf("    |_ [rect] x: %d, y: %d, w: %d, h: %d,\n", widget->rect->x, widget->rect->y, widget->rect->w, widget->rect->h);
    if (widget->texture != NULL) {
        printf("    |_ [default texture] %p\n", (void *)widget->texture);
    } else {
        printf("    |_ [default texture] NULL\n");
    }
    if (widget->htexture != NULL) {
        printf("    |_ [highlight texture] %p\n", (void *)widget->htexture);
    } else {
        printf("    |_ [highlight texture] NULL\n");
    }
}

/**
 * mouse detection for widget
 */
int widget_is_in_rect(int x, int y, SDL_Rect *rect) {
    return x >= rect->x && x < rect->x + rect->w && y >= rect->y && y < rect->y + rect->h;
}

/**
 * Initialize widget configuration with the minimal required properties
 */
WidgetConfig widget_configure(int id, WidgetType type, char *text) {
    WidgetConfig config = {
        .id = id,
        .type = type,
        .state = WSTATE_DEFAULT,
        .text = text,
        .fgColor = NULL,
        .bgColor = NULL,
        .borderColor = NULL,
        .fgColorH = NULL,
        .bgColorH = NULL,
        .borderColorH = NULL,
        .action = NULL,
        .paddingX = 5,
        .paddingY = 5,
        .wrap = 0
    };
    return config;
}

/**
 * Initialize empty widget
 */
Widget *widget_new() {
    Widget *widget = malloc(sizeof(Widget));
    RETURN_VAL_IF_ERR(widget == NULL, NULL, "couldn't allocate memory for Widget");
    widget->id = 0;
    widget->type = WTYPE_BUTTON;
    widget->state = WSTATE_DEFAULT;
    widget->rect = malloc(sizeof(SDL_Rect));
        if (widget->rect == NULL) {
            free(widget);
            LOG_ERROR("couldn't allocate memory for Widget rect");
            return NULL;
        };
    widget->rect->x = 0;
    widget->rect->y = 0;
    widget->rect->w = 0;
    widget->rect->h = 0;
    widget->texture = NULL;
    widget->htexture = NULL;
    widget->action = NULL;

    return widget;
}

/**
 * Deallocate widget and properties, destroy SDL textures
 */
void widget_destroy( Widget *widget ) {
    if (widget == NULL) {
        return;
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
}

/**
 * Build a widget textures and cache them in widget struct.
 * Do this before the entering the render loop
 * @return -1 on failure, otherwise 0
 */
Widget *widget_build(WidgetConfig config, SDL_Renderer *ren, TTF_Font *font) {
    Widget *widget = widget_new(config);
    widget->id = config.id;
    widget->type = config.type;
    widget->state = config.state;

    SDL_Color defaultFgCol = {0, 0, 0, 0};
    if (config.fgColor == NULL) {
        fprintf(stderr, "No foreground color set for widget (id %d)\n", config.id);
        config.fgColor = &defaultFgCol; // mutate
    }

    // default texture
    widget->texture = create_widget_texture(ren, font, config.text, config.type, config.fgColor, config.bgColor, config.borderColor, config.paddingX, config.paddingY, config.wrap);

    // highlight texture
    if (
        config.fgColorH     != NULL ||
        config.bgColorH     != NULL ||
        config.borderColorH != NULL
    ) {
        SDL_Color *fgColorH;
        SDL_Color *bgColorH;
        SDL_Color *borderColorH;
        fgColorH =     (config.fgColorH != NULL)     ? config.fgColorH : config.fgColor;
        bgColorH =     (config.bgColorH != NULL)     ? config.bgColorH : config.bgColor;
        borderColorH = (config.borderColorH != NULL) ? config.borderColorH : config.borderColor;
        widget->htexture = create_widget_texture(ren, font, config.text, config.type, fgColorH, bgColorH, borderColorH, config.paddingX, config.paddingY, config.wrap);
    }

    // update box geometry
    SDL_QueryTexture(widget->texture, NULL, NULL, &(widget->rect->w), &(widget->rect->h));

    return widget;
}

/**
 * Convenience method for setting widget position
*/
void widget_set_postion(Widget *widget, int x, int y) {
    widget->rect->x = x;
    widget->rect->y = y;
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
