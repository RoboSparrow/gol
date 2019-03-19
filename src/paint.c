#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gol.h"
#include "state.h"
#include "utils.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect rect;

SDL_Color bg_color = { 44, 74, 82, 255 }; // #2C4A52
SDL_Color rect_color = { 244, 235, 219, 255 }; // #F4EBDB
SDL_Color grid_color = { 10, 49, 57, 255 }; // bg_color darkened 10%

// text/ttf
TTF_Font *font;

SDL_Texture *text_texture;

SDL_Color text_color = { 134, 155, 151 };
int font_size = 10;


void draw_background(int cols, int rows) {
    int w = 0;
    int h = 0;
    SDL_RenderGetLogicalSize(renderer, &w, &h);

    int ws = w / cols;
    int hs = h / rows;

    int i;
    for (i = 0; i < cols; i++) {
        SDL_RenderDrawLine(renderer, ws * i, 0, ws * i, h);
    }
    SDL_RenderDrawLine(renderer, w, 0, w, h);

    for (i = 0; i < rows; i++) {
        SDL_RenderDrawLine(renderer, 0, hs * i, w, hs * i);
    }
    SDL_RenderDrawLine(renderer, 0, h, w, h);
}

////
// text
////

void font_init() {
    Path path;
    path_build("hellovetica.ttf", path);

    if(TTF_Init( )== -1) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    font = TTF_OpenFont(path, font_size);
    if (font == NULL) {
         fprintf(stderr, "unable to locate font in ./, trying ./bin\n");
         exit(EXIT_FAILURE);
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
        fprintf(stderr, "Failed to render text: %p\n", TTF_GetError());
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
    int sizeX = UNIT_SIZE * cols;
    int sizeY = UNIT_SIZE * rows;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        fprintf(stderr, " Failed to initialize SDL : %p\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // @see https://wiki.libsdl.org/SDL_WindowFlags
    window = SDL_CreateWindow( "RoboSparrow's game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizeX, sizeY, 0);
    if (window == NULL) {
        fprintf(stderr, "Failed to create window : %p\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // @see https://wiki.libsdl.org/SDL_RendererFlags
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL){
        fprintf(stderr, "Failed to create renderer : %p\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);

    font_init() ;

    rect.x = 0;
    rect.y = 0;
    rect.w = UNIT_SIZE;
    rect.h = UNIT_SIZE;
}

void paint_clear() {
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void paint_loop_start(int cols, int rows) {
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
    draw_background(cols, rows);
    SDL_SetRenderDrawColor(renderer, rect_color.r, rect_color.g, rect_color.b, rect_color.a);

}

void paint_loop_cell(int cell, int index, int cols, int rows) {
    // SDL_Log("cell index %d, status %d", index, cell);
    if (cell == GOL_ALIVE) {

        int row = gol_row(index, cols);
        int col = gol_col(index, row, cols);

        rect.x = UNIT_SIZE * col;
        rect.y = UNIT_SIZE * row;

        SDL_RenderFillRect(renderer, &rect);
    }
}

void paint_loop_row_end() {}

void paint_loop_end(int cols, int rows) {

    int rw = 0;
    int rh = 0;
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
