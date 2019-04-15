#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "pattern.h"
#include "utils.h"
#include "state.h"
#include "gol.h"
/**
 * helper input for init_parse_args -l
 */
static void cli_print_patternlist(GenList *list) {
    for(int i = 0; i < list->length; i++) {
        Pattern *pattern = (Pattern*) genlist_get(list, i);
        printf("%d) %s (%dx%d)\n", i + 1, pattern->title, pattern->cols, pattern->rows);
        printf("    * %s\n", pattern->description);
    }
}

/**
 * helper input for init_parse_args -l
 */
static int select_pattern(int len) {
    printf("--------------\n");
    int selected = 0;
    do {
        int cl;
        printf("select pattern: ");
        scanf("%d", &selected);
        while((cl = getchar()) != EOF && cl != '\n'); //clean the buffer

    } while(selected <= 0 || selected > len);
    return selected;
}

/**
 * Sets path to executable
 */
void init_set_rootdir(int argc, char *argv[], Path path) {
    // TODO this needs to be made compatible for OS and runtime modes
    if(*argv[0] == '/') {
        strcpy(path, dirname(argv[0]));
    } else {
        realpath(dirname(argv[0]), path);
    }
    if(access(path, R_OK)) {
       LOG_ERROR_F("init: could not define or access path to executable (%s).", path);
    }
}

/**
 * parses cli args in to world pattern
 * @param param file path to be filled with file path on option -f
 * @return 1 if skipping start screen and enter world screen directly or 0, -1 on error
 */
int init_parse_args(int argc, char* argv[], GlobalState *app, Pattern *world, Path patternfile) {
    int opt = 0;
    int cols = 0;
    int rows = 0;
    int createRandomWorld = 0;
    int skipstartscreen = 0;

    char usage[] = "usage: %s [-l listpatternsandselect] [-c numberofcols] [-r numberofrows] [-p pathtopatternfile][-R enterrandomworld]\n";
    while ((opt = getopt(argc, argv, "r:c:p:h:lRV")) != -1) {
        switch (opt)  {
            case 'c':
                cols = atoi(optarg);
            break;
            case 'r':
                rows = atoi(optarg);
            break;
            case 'l': {
                GenList patterns;
                genlist_init(&patterns);
                int loaded = pattern_load_patternlist("patterns", &patterns);
                EXIT_MINUS(loaded, "init: Unable to load pattern list.");
                cli_print_patternlist(&patterns);

                int selected = select_pattern(patterns.length);

                Pattern *pattern = (Pattern *) genlist_get(&patterns, selected - 1);
                printf("=> %s\n", pattern->file);
                strcpy(patternfile, pattern->file);

                // switch to world screen
                app->screen = SDL_SCREEN_WORLD;

                genlist_free(&patterns);
                // exit(EXIT_SUCCESS);
            }
            break;
            case 'p':
                path_build(optarg, patternfile);
            break;
            // directly go to random world
            case 'R':
                createRandomWorld = 1;
                skipstartscreen = 1;
            break;
            // valgrind: run once only
            case 'V':
                printf("-V option (valgrind) run loop once and exit\n");
                app->state = APP_STATE_ONCE;
            break;
            case 'h':
            case '?':
                fprintf(stderr, usage, argv[0]);
                exit(EXIT_SUCCESS);
            break;
        }
    }

    world->cols = (cols > 0) ? cols : 50;
    world->rows = (rows > 0) ? rows : world->cols;

    if (createRandomWorld) {
        pattern_create_random(world, PATTERN_FULL);
    }

    return skipstartscreen;
    // pattern_print_pattern(world);
}
