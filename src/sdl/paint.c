#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gol.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect rect;

SDL_Color bg_color = { 44, 74, 82, 255 };
SDL_Color rect_color = { 244, 235, 219, 255 };

int rect_size = 10;

////
// text
////

// text/ttf
TTF_Font *font;

SDL_Texture *text_texture;

SDL_Color text_color = { 134, 155, 151 };
int font_size = 10;

void font_init() {
    if(TTF_Init( )== -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
        exit(2);
    }

    font = TTF_OpenFont("hellovetica.ttf", font_size);
    if (font == NULL) {
        printf("TTF_OpenFont error: %s\n", TTF_GetError());
        exit(2);
    }
}

void font_exit() {
    if(font) {
        TTF_CloseFont(font);
    }

    if(text_texture) {
        SDL_DestroyTexture(text_texture);
    }
}

SDL_Rect render_text(char text[]) {
    int w = 0;
    int h = 0;
    SDL_Rect text_rect = { 0, 0, w, h };
    SDL_Surface *text_surface;

    if (!(text_surface = TTF_RenderUTF8_Solid(font, text, text_color))) {
        printf("Failed to render text: %p", TTF_GetError());
        return text_rect;
    } else {
        text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
        text_rect.w = w;
        text_rect.h = h;
        SDL_FreeSurface(text_surface);
        return text_rect;
    }
}

////
//  interfaces
////

void paint_init(int cols, int rows) {
    int sizeX = rect_size * cols;
    int sizeY = rect_size * rows;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        printf(" Failed to initialize SDL : %p", SDL_GetError());
        exit(1);
    }

    // @see https://wiki.libsdl.org/SDL_WindowFlags
    window = SDL_CreateWindow( "RoboSparrow's game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
    if (window == NULL) {
        printf("Failed to create window : %p", SDL_GetError());
        exit(1);
    }

    // @see https://wiki.libsdl.org/SDL_RendererFlags
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL){
        printf("Failed to create renderer : %p", SDL_GetError());
        exit(1);
    }

    SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);

    font_init() ;

    rect.x = 0;
    rect.y = 0;
    rect.w = rect_size;
    rect.h = rect_size;
}

void paint_clear() {
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void paint_loop_start(int cols, int rows) {
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
}

void paint_loop_cell(int cell, int index, int cols, int rows) {
    // SDL_Log("cell index %d, status %d", index, cell);
    if (cell == GOL_ALIVE) {

        int row = gol_row(index, cols);
        int col = gol_col(index, row, cols);

        rect.x = rect_size * col;
        rect.y = rect_size * row;

        SDL_RenderFillRect(renderer, &rect);
    }
}

void paint_loop_row_end() {}

void paint_loop_end(int cols, int rows) {

    int rw;
    int rh;
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
    SDL_Rect text_rect = render_text("press enter for restart, space for pause");//TODO daw only once and cache
    text_rect.x = (rw - text_rect.w) / 2;
    text_rect.y = rh - text_rect.h - 10;

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderPresent(renderer);
}

void paint_exit() {
    font_exit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
