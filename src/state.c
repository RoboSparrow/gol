#include <SDL2/SDL.h>

#include "state.h"
#include "utils.h"
#include "gol.h"

/* define externals */

/**
 * colour scheme: https://material.io/tools/color/#!/?view.left=0&view.right=0&primary.color=546E7A&secondary.color=FFAB40
 */
SdlColors Colors = {
    .bg = { 41,67,78 }, // P-dark
    .fg = { 129, 156, 169 }, // P-Light
    .contrast = { 255, 171, 64 }, // S
    .grid = { 69, 90, 100 }, // P (-1)
    .borders = { 129, 156, 169 }, // P-Light
    .text = { 207, 216, 220 } // Blue Grey(2)
};

SdlFonts Fonts; //populated by renderer

int game_start(GlobalState *App, Pattern *world) {
    // autosave world
    int saved = pattern_save_file(AUTOSAVE_FILE, world);
    if (saved == -1) {
        LOG_ERROR_F("Could not save pattern file: %s", AUTOSAVE_FILE);
    }
    App->screen = SDL_SCREEN_WORLD;
    if(App->state < APP_STATE_ONCE) {// todo simplify
        App->state = APP_STATE_RUNNING;
    }
    return saved;
}

int game_restart(GlobalState *App, Pattern *world) {
    gol_clear_data(world->data);
    int loaded = pattern_load_file(AUTOSAVE_FILE, world, PATTERN_FULL);
    if (loaded == -1) {
        LOG_ERROR_F("Could not load pattern file: %s", AUTOSAVE_FILE);
    }
    App->screen = SDL_SCREEN_WORLD;
    if(App->state < APP_STATE_ONCE) {// todo simplify
        App->state = APP_STATE_RUNNING;
    }
    return loaded;
}

// unused
int game_save(Pattern *world, char *file) {
    int saved = pattern_save_file(file, world);
    if (saved == -1) {
        LOG_ERROR_F("Could not save pattern file: %s", file);
    }
    return saved;
}

int game_load(Pattern *world, char *file){
    gol_clear_data(world->data);
    int loaded = pattern_load_file(file, world, PATTERN_FULL);
    if (loaded == -1) {
        LOG_ERROR_F("Could not load pattern file: %s", file);
    }
    return loaded;
}

/**
 * Load pattern from file and merge into target
 * @param file file to load
 * @param pattern allocated target Pattern to lmerge into
 * @param offsetX x-offset for instering into target pattern
 * @param offsetY y-offset for instering into target pattern
 * @param origin flag for alignment of pattern relative to world offset
 * @return -1 on error or 0
 */
int game_merge(Pattern *world, char *file, int offsetX, int offsetY, PatternOrigin origin) {
    gol_clear_data(world->data);
    Pattern *pattern = pattern_allocate_pattern();
    if (pattern == NULL) {
        LOG_ERROR_F("Could not allocate memory for pattern file %s\n", file);
        return -1;
    }

    int loaded = pattern_load_file(file, pattern, PATTERN_FULL);
    if (loaded < 0) {
        LOG_ERROR_F("Could not load pattern file %s\n", file);
        return -1;
    }

    switch (origin) {
        case PATTERN_CENTER:
            gol_merge_data(pattern, world, offsetX - (pattern->cols/2), offsetY  - (pattern->rows/2));
        break;
        default:
            gol_merge_data(pattern, world, offsetX, offsetY);
    }

    pattern_free_pattern(pattern);
    return 0;
}
