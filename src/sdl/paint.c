#include <SDL2/SDL.h>

#include "gol.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect rect;

int bg_color[4] = { 56, 82, 102, 255 };
int rect_color[4] = { 226, 192, 68, 255 };
int rect_size = 20;

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

    // Set size of renderer to the same as window
    SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);
    // set bg color
    SDL_SetRenderDrawColor(renderer, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

    rect.x = 0;
    rect.y = 0;
    rect.w = rect_size;
    rect.h = rect_size;
}

void paint_clear() {
    // Change color to blue
    SDL_SetRenderDrawColor(renderer, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
    // Clear the window and make it all green
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void paint_loop_start(int cols, int rows) {
    // Clear the window
    SDL_RenderClear(renderer);
    // Set drawing color
    SDL_SetRenderDrawColor(renderer, rect_color[0], rect_color[1], rect_color[2], rect_color[3]);
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
    // Change color to green
    SDL_SetRenderDrawColor(renderer, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
    // Render the changes above
    SDL_RenderPresent(renderer);
}

void paint_exit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
