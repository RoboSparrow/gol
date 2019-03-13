// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <SDL2/SDL.h>

#include "gol.h"
#include "paint.h"
#include "state.h"

int main(int argc, char* argv[]) {
    int running = 1;
    int paused = 0;

    int cols = 0;
    int rows = 0;

    if (argc > 1) {
        cols = atoi(argv[1]);
    }

    if (argc > 2) {
        rows = atoi(argv[2]);
    }

    if (cols <= 0) {
        cols = 50;
    }

    if (rows <= 0) {
        rows = cols;
    }

    int size = cols * rows;
    char *world = calloc(size, sizeof(char));
    if(world == NULL) {
        fprintf(stderr, "Not enough memory for allocating world data");
    }
    
    gol_init(world, cols, rows);
    paint_init(cols, rows);

    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                    break;
                    case SDLK_SPACE:
                        paused = !paused;
                    break;
                    case SDLK_RETURN:
                        paint_clear();
                        paused = 0;
                        SDL_Delay(1000);
                        gol_init(world, cols, rows);
                    break;
                }
            }
        }

        if (!paused) {
            paint_loop_start(cols, rows);
            gol_update(world, cols, rows);
            paint_loop_end(cols, rows);
        }

        SDL_Delay(100); // 10 fps: 100
    }

    paint_exit(cols, rows);
    
    if (world != NULL) {
        free(world);
    }

    return 0;
}
