// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/

#include <SDL2/SDL.h>
#include <stdio.h>

#include "gol.h"
#include "paint.h"

int main(int argc, char* argv[]) {
    int running = 1;

    int cols = 0;
    int rows = 0;

    if (argc > 1) {
        cols = atoi(argv[1]);
    }

    if (argc > 2) {
        rows = atoi(argv[2]);
    }

    if (cols <= 0) {
        cols = 30;
    }

    if (rows <= 0) {
        rows = cols;
    }

    int size = cols * rows;
    int world[size];

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
                    case SDLK_RETURN:
                        paint_clear();
                        SDL_Delay(1000);
                        gol_init(world, cols, rows);
                    break;
                }
            }
        }

        paint_loop_start(cols, rows);
        gol_update(world, cols, rows);
        paint_loop_end(cols, rows);
        SDL_Delay(100); // 10 fps
    }

    paint_exit(cols, rows);
    return 0;
}
