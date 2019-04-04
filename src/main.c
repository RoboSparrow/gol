#ifndef __DOING_TESTS__
// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "utils.h"
#include "gol.h"
#include "renderer.h"
#include "state.h"
#include "rle-parser.h"
#include "pattern.h"
#include "init.h"
#include "state.h"
#include "renderer.h"

Path FS_ROOT_DIR;

int main(int argc, char* argv[]) {

    GlobalState App = {
        .state = APP_STATE_RUNNING,
        .screen = SDL_SCREEN_START,
    };

    int paused = 0;

    Path patternfile = "";
    SDL_Event e;

    // root dir, TODO this needs to be made compatible for OS and runtime modes
    init_set_rootdir(argc, argv, FS_ROOT_DIR);

    // init world meta
    Pattern *world = pattern_allocate_pattern();
    EXIT_NULL(world, "could not allocate memory(1) for world pattern");

    // merge args
    init_parse_args(argc, argv, &App, world, patternfile);

    // init world data
    world->data = gol_allocate_data(world->cols, world->rows);
    EXIT_NULL(world->data, "could not allocate memory(2) for world pattern");

    // populate world
    if(patternfile[0] == '\0') {
        // random world
        gol_random(world);
    } else {
        int merged = pattern_load_file_and_merge(patternfile, world, world->cols / 2, world->rows / 2, PATTERN_CENTER);
        EXIT_MINUS(merged, "main: could not load pattern file\n");
    }

    // autosave world

    int saved = pattern_save_file("save/autosave.rle", world);
    EXIT_MINUS(saved, "main: could not save pattern file (autosave)\n");

    // init sdl & screens
    renderer_init(world);
    screen_start_init();
    screen_world_init();

    // add a small delay so that return key event from cli is not captured
    // TODO use var
    SDL_Delay(100);

    while (App.state > APP_STATE_QUIT) {
        renderer_update();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                App.state  = APP_STATE_QUIT;
            }

            switch (App.screen) {
                case SDL_SCREEN_WORLD:
                    screen_world_events(e, &App, world);
                break;

                case SDL_SCREEN_START:
                default:
                    screen_start_events(e, &App, world);
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        App.screen = SDL_SCREEN_START;
                    break;
                    case SDLK_SPACE:
                        paused = !paused;
                    break;
                    case SDLK_RETURN:
                        screen_world_clear();
                        paused = 0;
                        SDL_Delay(200);
                            if(strlen(patternfile) == 0) {
                                // random world
                                gol_random(world);
                            } else {
                                int merged = pattern_load_file_and_merge(patternfile, world, world->cols / 2, world->rows / 2, PATTERN_CENTER);
                                EXIT_MINUS(merged, "main: could not load pattern file\n");
                            }
                    break;
                }
            }
        }

        switch (App.screen) {
            case SDL_SCREEN_WORLD:
                if (!paused) {
                    screen_world_render(world);
                }
            break;

            case SDL_SCREEN_START:
            default:
                screen_start_render(world);
        }

        SDL_Delay(50); // 15 fps

        // DEV valgrind with SDL
        if(App.state == APP_STATE_ONCE) {
            exit(0);
        }
    }

    // exit screens sdl
    screen_start_destroy();
    screen_world_destroy();
    renderer_destroy();

    // free data
    pattern_free_pattern(world);

    return 0;
}
#endif
