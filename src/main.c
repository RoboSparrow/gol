// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "gol.h"
#include "paint.h"
#include "state.h"

char *patternfile = "";

Pattern parse_args(int argc, char* argv[]) {
    int opt;
    int cols = 0;
    int rows = 0;


    char usage[] = "usage: %s [-c numberofcols] [-r numberofrows] [-p patternfile]\n";
    while ((opt = getopt (argc, argv, "r:c:p:h")) != -1) {
        switch (opt)  {
            case 'c':
                cols = atoi(optarg);
            break;
            case 'r':
                rows = atoi(optarg);
            break;
            case 'p':
                patternfile = optarg;
            break;
            case 'h':
                fprintf(stderr, usage, argv[0]);
                exit(EXIT_SUCCESS);
            break;
            case '?':
                fprintf(stderr, usage, argv[0]);
                exit(EXIT_FAILURE);
            break;
        }
    }

    if(strlen(patternfile) > 0) {
        if (access(patternfile, F_OK)) {
            fprintf(stderr, "cannot load pattern file %s\n", patternfile);
            exit(EXIT_FAILURE);
        }
    }

    if (cols <= 0) {
        cols = 50;
    }

    if (rows <= 0) {
        rows = cols;
    }

    Pattern pattern = {
        .cols = cols,
        .rows = rows
    };
    return pattern;
}

int main(int argc, char* argv[]) {
    int running = 1;
    int paused = 0;

    Pattern pattern = parse_args(argc, argv);
    int cols = pattern.cols;
    int rows = pattern.rows;

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
