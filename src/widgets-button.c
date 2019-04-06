#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "widgets.h"
#include "renderer.h"

extern SDL_Window* window;

static SDL_Surface *create_rect_surface(SDL_Rect *rect, SDL_Color *bgcolor, SDL_Color *bordercolor) {
    SDL_Surface *s;

    s = SDL_CreateRGBSurfaceWithFormat(0, rect->w, rect->h, 32, SDL_GetWindowPixelFormat(window));
    if (s == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return NULL;
    }
    SDL_SetSurfaceBlendMode(s, SDL_BLENDMODE_NONE);

    Uint32 ubgcol = SDL_MapRGB(s->format, bgcolor->r, bgcolor->b, bgcolor->g);
    SDL_FillRect(s, NULL, ubgcol);

    if(bordercolor != NULL) {
        Uint32 ubdrcol = SDL_MapRGB(s->format, bordercolor->r, bordercolor->b, bordercolor->g);

        SDL_Rect top = { 0, 0, rect->w, 1 };
        SDL_FillRect(s, &top, ubdrcol);

        SDL_Rect left = { 0, 0, 1, rect->h };
        SDL_FillRect(s, &left, ubdrcol);

        SDL_Rect right = { rect->w - 1, 0, 1, rect->h };
        SDL_FillRect(s, &right, ubdrcol);

        SDL_Rect bottom = { 0, rect->h - 1 , rect->w, 1 };
        SDL_FillRect(s, &bottom, ubdrcol);
    }

    return s;
}

void widgets_button_init(Widget *btn, SDL_Renderer *ren, TTF_Font *font, int x, int y) {
    int margin = 5;

    // default texture

    SDL_Surface *fgs = TTF_RenderText_Blended(font, btn->text, btn->text_color);
    if (fgs == NULL) {
        SDL_Log("TTF_RenderText_Solid failed: %s", SDL_GetError());
        return;
    }

    SDL_Rect fgr = {  margin, margin, fgs->w, fgs->h };
    SDL_Rect bgr = { x, y, fgs->w + (2 * margin), fgs->h + (2 * margin) };

    SDL_Surface *bgs = create_rect_surface(&bgr, &(btn->bg_color), &(btn->border_color));
    if (SDL_BlitSurface(fgs, NULL, bgs, &fgr)) {
        SDL_Log("SDL_BlitSurface() failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *default_texture = SDL_CreateTextureFromSurface(ren, bgs);
    if (default_texture == NULL) {
        SDL_Log("Rendering btn texture failed: %s", SDL_GetError());
        return;
    }

    // hover texture

    SDL_Surface *h_fgs = TTF_RenderText_Blended(font, btn->text, btn->bg_color);
    SDL_Surface *h_bgs = create_rect_surface(&bgr, &(btn->text_color), &(btn->border_color));

    if (SDL_BlitSurface(h_fgs, NULL, h_bgs, &fgr)) {
        SDL_Log("SDL_BlitSurface() failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *hover_texture = SDL_CreateTextureFromSurface(ren, h_bgs);
    if (hover_texture == NULL) {
        SDL_Log("Rendering btn texture failed: %s", SDL_GetError());
        return;
    }

    // assign geometry

    int w = 0;
    int h = 0;
    SDL_QueryTexture(default_texture , NULL, NULL, &w, &h);
    SDL_Rect rect = { x, y, w, h };

    btn->rect = rect;
    btn->default_texture = default_texture;
    btn->hover_texture = hover_texture;

    // cleanup

    SDL_FreeSurface(fgs);
    SDL_FreeSurface(bgs);
    SDL_FreeSurface(h_fgs);
    SDL_FreeSurface(h_bgs);
}

void widgets_button_render(Widget *btn, SDL_Renderer *ren) {
    int ok = 0;

    switch(btn->state) {

        case WSTATE_DEFAULT:
            if(btn->default_texture != NULL) {
                ok = SDL_RenderCopy(ren, btn->default_texture, NULL, &(btn->rect));
            }
            break;

        case WSTATE_HOVER:
        case WSTATE_ACTIVE:
        case WSTATE_RELEASED:
            if(btn->hover_texture != NULL) {
                ok = SDL_RenderCopy(ren, btn->hover_texture, NULL, &(btn->rect));
            }
            break;

        case WSTATE_NONE:
        default:
            break;
    }

    if (ok < 0) {
        SDL_Log("SDL_RenderCopy() failed: %s", SDL_GetError());
    }
}

void widgets_btn_event(Widget *btn, SDL_Event e) {
    if(!widgets_is_in_rect(e.motion.x, e.motion.y, &(btn->rect))) {
        btn->state = WSTATE_DEFAULT;
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        btn->state = WSTATE_ACTIVE;
        return;
    }

    if (e.type == SDL_MOUSEBUTTONUP) {
        if(btn->state == WSTATE_ACTIVE) {
            btn->state = WSTATE_RELEASED;
            return;
        }
        btn->state = WSTATE_DEFAULT;
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        btn->state = WSTATE_HOVER;
        return;
    }

}

void widgets_button_destroy(Widget *btn) {
    if (btn->default_texture != NULL) {
        SDL_DestroyTexture(btn->default_texture);
    }
    if (btn->hover_texture != NULL) {
        SDL_DestroyTexture(btn->hover_texture);
    }
    btn = NULL;
}
