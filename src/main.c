// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <libgen.h>

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

    // root dir
    // TODO this needs to be made os and runtime compatible
    if(*argv[0] == '/') {
        strcpy(FS_ROOT_DIR, argv[0]);
    } else {
        realpath(argv[0], FS_ROOT_DIR);
    }
    dirname(FS_ROOT_DIR);

    // init world meta
    Pattern *meta = pattern_allocate_pattern();
    if (meta == NULL) {
        // todo macro EXIT_NULL(meta, message)
        return EXIT_FAILURE;
    }

    // merge args
    Path patternfile = "";
    init_parse_args(argc, argv, meta, patternfile);

    int cols = meta->cols;
    int rows = meta->rows;

    // init world data
    char *world = gol_allocate_data(cols, rows);
    if (world == NULL) {
        return EXIT_FAILURE;
    }

    if(strlen(patternfile) == 0) {
        // random world
        gol_init(world, cols, rows);
    } else {
        // pattern from file
        int merged = pattern_merge_from_file(patternfile, "rle",  world, cols, rows, 2, 2);
        if(merged <= 0) {
            return EXIT_FAILURE;
        }
    }

    paint_init(cols, rows);
    SDL_Event e;

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
                                gol_init(world, cols, rows);
                            } else {
                                // pattern from file
                                gol_clear_data(world);
                                pattern_merge_from_file(patternfile, "rle",  world, cols, rows, 2, 2); // not checking for failure again
                            }
                    break;
                }
            }
        }

        if (!paused) {
            paint_loop_start(cols, rows);
            gol_update(world, cols, rows);
            paint_loop_end(cols, rows);
        }

        SDL_Delay(50); // 15 fps
    }

    running = 0;
    paint_exit(cols, rows);
    pattern_free_pattern(meta);
    gol_free_data(world);

    return 0;
}
