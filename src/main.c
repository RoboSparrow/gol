// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "utils.h"
#include "gol.h"
#include "paint.h"
#include "state.h"
#include "rle-parser.h"
#include "pattern.h"

#include "init.h"

Path FS_ROOT_DIR;

int main(int argc, char* argv[]) {

    int running = 1;
    int paused = 0;
    Path patternfile = "";
    SDL_Event e;

    // root dir, TODO this needs to be made compatible for OS and runtime modes
    init_set_rootdir(argc, argv, FS_ROOT_DIR);

    // init world meta
    Pattern *world = pattern_allocate_pattern();
    EXIT_NULL(world, "could not allocate memory(1) for world pattern");

    // merge args
    init_parse_args(argc, argv, world, patternfile);

    // init world data
    world->data = gol_allocate_data(world->cols, world->rows);
    EXIT_NULL(world->data, "could not allocate memory(2) for world pattern");

    // populate world
    if(patternfile[0] == '\0') {
        // random world
        gol_init(world->data, world->cols, world->rows);
    } else {
        // pattern from file
        int merged = pattern_merge_from_file(patternfile, "rle",  world->data, world->cols, world->rows, 2, 2);
        if(merged <= 0) {
            return EXIT_FAILURE;
        }
    }

    paint_init(world->cols, world->rows);

    // add a small delay so that return key event from cli is not captured
    // TODO use var
    SDL_Delay(100);

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
                        SDL_Delay(200);
                            if(strlen(patternfile) == 0) {
                                // random world
                                gol_init(world->data, world->cols, world->rows);
                            } else {
                                // pattern from file
                                gol_clear_data(world->data);
                                pattern_merge_from_file(patternfile, "rle", world->data, world->cols, world->rows, 2, 2); // not checking for failure again
                            }
                    break;
                }
            }
        }

        if (!paused) {
            paint_loop_start(world->cols, world->rows);
            gol_update(world->data, world->cols, world->rows);
            paint_loop_end(world->cols, world->rows);
        }

        SDL_Delay(50); // 15 fps
    }

    running = 0;
    paint_exit(world->cols, world->rows);

    gol_free_data(world->data);
    pattern_free_pattern(world);

    return 0;
}
