#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "widgets.h"

SDL_Surface *create_rect_surface(SDL_Rect *rect, SDL_Color *col) {
    SDL_Surface *s;
    s = SDL_CreateRGBSurface(0, rect->w, rect->h, 32, 0, 0, 0, 0);
    if (s == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return NULL;
    }
    SDL_FillRect(s, NULL, SDL_MapRGB(s->format, col->r, col->b, col->g));
    return s;
}

Widget *widgets_button_new(char *text) {
    Widget _btn = {
        .state = WSTATE_DEFAULT,
        .text = text,
        .text_color = { 0, 0, 0 },
        .bg_color = { 255, 255, 255 }
    };
    Widget *btn = &_btn;
    return btn;
}

void widgets_button_init(Widget *btn, SDL_Renderer *ren, TTF_Font *font, int x, int y) {
    int margin = 5;

    SDL_Surface *fgs = TTF_RenderText_Solid(font, btn->text, btn->text_color);
    if (fgs == NULL) {
        SDL_Log("TTF_RenderText_Solid failed: %s", SDL_GetError());
        return;
    }

    SDL_Rect fgr = {  margin, margin, fgs->w, fgs->h };
    SDL_Rect bgr = { x, y, fgs->w + (2 * margin), fgs->h + (2 * margin) };

    SDL_Surface *bgs = create_rect_surface(&bgr, &(btn->bg_color));
    if (SDL_BlitSurface(fgs, NULL, bgs, &fgr)) {
        SDL_Log("SDL_BlitSurface() failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *default_texture = SDL_CreateTextureFromSurface(ren, bgs);
    if (default_texture == NULL) {
        SDL_Log("Rendering btn texture failed: %s", SDL_GetError());
        return;
    }

    SDL_Surface *h_fgs = TTF_RenderText_Solid(font, btn->text, btn->bg_color);
    SDL_Surface *h_bgs = create_rect_surface(&bgr, &(btn->text_color));

    if (SDL_BlitSurface(h_fgs, NULL, h_bgs, &fgr)) {
        SDL_Log("SDL_BlitSurface() failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *hover_texture = SDL_CreateTextureFromSurface(ren, h_bgs);
    if (hover_texture == NULL) {
        SDL_Log("Rendering btn texture failed: %s", SDL_GetError());
        return;
    }

    int w = 0;
    int h = 0;
    SDL_QueryTexture(default_texture , NULL, NULL, &w, &h);
    SDL_Rect rect = { x, y, w, h };

    btn->rect = rect;
    btn->default_texture = default_texture;
    btn->hover_texture = hover_texture;

    SDL_FreeSurface(fgs);
    SDL_FreeSurface(bgs);
    SDL_FreeSurface(h_fgs);
    SDL_FreeSurface(h_bgs);
}

void widgets_button_render(Widget *btn, SDL_Renderer *ren) {
    switch(btn->state) {
        case WSTATE_DEFAULT:
            SDL_RenderCopy(ren, btn->default_texture, NULL, &(btn->rect));
            break;
        case WSTATE_HOVER:
        case WSTATE_ACTIVE:
            SDL_RenderCopy(ren, btn->hover_texture, NULL, &(btn->rect));
            break;
        case WSTATE_NONE:
        default:
            break;
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
