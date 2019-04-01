#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "pattern.h"
#include "utils.h"
#include "state.h"

/**
 * helper input for init_parse_args -l
 */
void cli_print_patternlist(PatternList *list) {
    for(int i = 0; i < list->len; i++) {
        printf("%d) %s (%dx%d)\n", i + 1, list->patterns[i]->title, list->patterns[i]->cols, list->patterns[i]->rows);
        printf("    * %s\n", list->patterns[i]->description);
    }
}

/**
 * helper input for init_parse_args -l
 */
int select_pattern(int len) {
    printf("--------------\n");
    int selected = 0;
    int cl;
    do {
        printf("select pattern: ");
        scanf("%d", &selected);
        while((cl = getchar()) != EOF && cl != '\n'); //clean the buffer

    } while(selected <= 0 || selected > len);
    return selected;
}

/**
 * Sets path to executable
 */
void init_set_rootdir(int argc, char* argv[], Path path) {
    // TODO this needs to be made compatible for OS and runtime modes
    if(*argv[0] == '/') {
        strcpy(path, argv[0]);
    } else {
        realpath(argv[0], path);
    }
    strcpy(path, dirname(path));
    if(access(path, R_OK)) {
       LOG_ERROR_F("init: could not define or access path to executable (%s).", path);
    }
}

/**
 * parses cli args in to world pattern
 * @param param file path to be filled with file path on option -f
 */
void init_parse_args(int argc, char* argv[], GlobalState *app, Pattern *world, Path patternfile) {
    int opt = 0;
    int cols = 0;
    int rows = 0;

    char usage[] = "usage: %s [-l listpatternsandselect] [-c numberofcols] [-r numberofrows] [-p pathtopatternfile]\n";
    while ((opt = getopt (argc, argv, "r:c:p:h:l")) != -1) {
        switch (opt)  {
            case 'c':
                cols = atoi(optarg);
            break;
            case 'r':
                rows = atoi(optarg);
            break;
            case 'l': {
                PatternList list = { 0, NULL };
                int loaded = pattern_load_patternlist("patterns", &list);
                EXIT_MINUS(loaded, "init: Unable to load pattern list.");
                cli_print_patternlist(&list);

                int selected = select_pattern(list.len);

                printf("=> %s\n", list.patterns[selected - 1]->file);
                strcpy(patternfile, list.patterns[selected - 1]->file);

                // switch to world screen
                app->screen = SDL_SCREEN_WORLD;

                pattern_free_patternlist(&list);
                // exit(EXIT_SUCCESS);
            }
            break;
            case 'p':
                path_build(optarg, patternfile);
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
    // pattern_print_pattern(world);
}
