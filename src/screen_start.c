#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <malloc.h>

#include "state.h"
#include "renderer.h"
#include "widget.h"
#include "pattern.h"
#include "gol.h"

extern SDL_Renderer *renderer;
extern SdlColors Colors;
extern SdlFonts Fonts;
extern RendererInfo rendererInfo;
extern GlobalState *App;

Widget *btn_rand = NULL;
Widget *btn_quit = NULL;

GenList patterns;
GenList patternWidgets;

int wid = 0; //todo static in widget func

int previewId = -1;
SDL_Texture *previewTexture = NULL;

#ifndef STARTSCREEN_PREVIEW_W
    #define STARTSCREEN_PREVIEW_W 200
#endif

int previewX = 0;
int previewY = 0;

////
// control buttons
////

/**
 * events for "enter" button
 * @param e SDL Event from main()
 * @param world world pattern
 */
static void events_btn_rand(SDL_Event e, Pattern *world) {
    widget_event(btn_rand, e);

    switch(btn_rand->state) {
        case WSTATE_RELEASED:
            printf("Random game...\n");
            gol_clear_data(world->data);
            gol_random(world);
            SDL_Delay(50);
            game_start(world);
            return;
        default:
            return;
    }
}

/**
 * events for "quit" button
 * @param e SDL Event from main()
 * @param world world pattern
 */
static void events_btn_quit(SDL_Event e, Pattern *world) {
    widget_event(btn_quit, e);

    switch(btn_quit->state) {
        case WSTATE_RELEASED:
            printf("Quitting app..\n");
            SDL_Delay(250);
            App->state = APP_STATE_QUIT;
            return;
        default:
            return;
    }
}

////
// pattern widgets
////

/**
 * initializes and builds pattern widgets on screen enter
 * @param startX x position for group on canvas
 * @param startY y position for group on canvas
 * @param xMargin x margin for individiual widget
 * @param yMargin y margin for individiual widget
 */
static void init_pattern_widgets(int startX, int startY, int xMargin, int yMargin) {
    int x = startX;
    int y = startY;

    if(!patterns.length) {
        return;
    }

    for (int i = 0; i < patterns.length; i++) {
        Pattern *pattern = (Pattern *) genlist_get(&patterns, i);

        WidgetConfig config = widget_configure(wid++, WTYPE_BUTTON, pattern->title);
        config.fgColor = &(Colors.text);
        config.bgColor = &(Colors.bg);
        config.borderColorH = &(Colors.contrast);
        Widget *widget = widget_build(config, renderer, Fonts.body);
        widget_set_postion(widget, x, y);
        genlist_push(&patternWidgets, widget);

        int height = widget->rect->h + yMargin;
        y += height;
    }
}

/**
 * renders pattern widgets
 */
static void render_pattern_widgets() {
    if(!patternWidgets.length) {
        return;
    }

    for(int i = 0; i < patternWidgets.length; i++) {
        Widget *widget = (Widget *) genlist_get(&patternWidgets, i);
        // widget_print_widget(widget);
        widget_render(widget, renderer, NULL);
    }
}

/**
 * renders a pattern preview into a texture
 */
static void load_pattern_preview(Pattern *pattern) {
    int margin = 5;
    int dim = (2 * margin) + STARTSCREEN_PREVIEW_W;

    if(pattern == NULL) {
        LOG_ERROR("preview pattern is NULL");
        return;
    }

    int loaded = pattern_load_file(pattern->file, pattern, PATTERN_FULL);
    if(loaded < 0) {
        LOG_ERROR_F("could not load pattern file for preview: %s", pattern->file);
        return;
    }

    SDL_Rect descriptionRect = { margin, margin, STARTSCREEN_PREVIEW_W, 0 };
    SDL_Rect patternRect = { margin, margin, STARTSCREEN_PREVIEW_W, STARTSCREEN_PREVIEW_W };
    SDL_Rect statsRect = { margin, margin, STARTSCREEN_PREVIEW_W, 0 };

    // description

    SDL_Texture *description = NULL;
    if(strlen(pattern->description)) {
        SDL_Surface *descSurface = TTF_RenderText_Blended_Wrapped(Fonts.body, pattern->description, Colors.text, STARTSCREEN_PREVIEW_W);
        if(descSurface != NULL) {
            description = SDL_CreateTextureFromSurface(renderer, descSurface);
        }
        if (description != NULL) {
            descriptionRect.w = descSurface->w;
            descriptionRect.h = descSurface->h;
            patternRect.y = descSurface->h + (2 * margin);
        }
    }

    // stats
    SDL_Texture *stats = NULL;
    char statStr[100];
    snprintf(statStr, 100, "columns: %d, rows: %d", pattern->cols, pattern->rows);
    SDL_Surface *statsSurface = TTF_RenderText_Blended_Wrapped(Fonts.body, statStr, Colors.text, STARTSCREEN_PREVIEW_W);
    if(statsSurface != NULL) {
        stats = SDL_CreateTextureFromSurface(renderer, statsSurface);
    }
    if (stats != NULL) {
        statsRect.w = statsSurface->w;
        statsRect.h = statsSurface->h;
        statsRect.y = patternRect.y + patternRect.h + margin;
    }

    int height = statsRect.y + statsRect.h + margin;
    previewTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dim, height);
    if(previewTexture == NULL) {
        LOG_ERROR_F("could not create preview texture for pattern file: %s", pattern->file);
        return;
    }

    SDL_SetTextureBlendMode(previewTexture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, previewTexture);

    renderer_set_color(Colors.grid);
    SDL_RenderClear(renderer);

    if (description != NULL) {
        SDL_RenderCopy(renderer, description, NULL, &descriptionRect);
    }
    //render pattern
    render_pattern_static(pattern, patternRect, Colors.contrast, Colors.bg, Colors.text);

    if (stats != NULL) {
        SDL_RenderCopy(renderer, stats, NULL, &statsRect);
    }
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, NULL);
}

/**
 * events for pattern widgets
 * @param e SDL Event from main()
 * @param world world pattern
 */
static void events_pattern_widgets(SDL_Event e, Pattern *world)  {
    if(!patternWidgets.length) {
        return;
    }

    int hasHovers = 0;

    for(int i = 0; i < patternWidgets.length; i++) {
        Widget *widget = (Widget *) genlist_get(&patternWidgets, i);
        Pattern *pattern = (Pattern *) genlist_get(&patterns, i);

        // default events
        widget_event(widget, e);

        // actions
        switch(widget->state) {
            case WSTATE_HOVER:
                hasHovers = 1;
                if(previewId != i) {
                    previewId = i;
                    // todo check against renderer w. h
                    previewX = widget->rect->x + widget->rect->w + 20;
                    previewY = widget->rect->y;
                    load_pattern_preview(pattern);
                }
            break;
            case WSTATE_RELEASED:
                printf("action widget (id:%d): %s..\n", widget->id, pattern->file);
                int merged = game_merge(world, pattern->file, world->cols / 2, world->rows / 2, PATTERN_CENTER);
                EXIT_MINUS(merged, "main: could not load pattern file"); // TODO screen message!
                int started = game_start(world);
                EXIT_MINUS(started, "Could not restart game"); // TODO screen message!
            break;

            default:
            break;
        }

    }

    if (!hasHovers) {
         previewId = -1;
         previewTexture = NULL;
    }
}


////
// screen api
////

/**
 * initialize this screen
 * @param world world pattern
 */
void screen_start_init(Pattern *world) {

    WidgetConfig config;

    // control buttons
    config = widget_configure(wid++, WTYPE_BUTTON, "Random game");
    config.fgColor = &(Colors.bg);
    config.bgColor = &(Colors.text);
    config.bgColorH = &(Colors.contrast);
    config.paddingX = 10;
    btn_rand = widget_build(config, renderer, Fonts.body);
    widget_set_postion(btn_rand, 5, 5);

    config = widget_configure(wid++, WTYPE_BUTTON, "Quit Game");
    config.fgColor = &(Colors.bg);
    config.bgColor = &(Colors.text);
    config.bgColorH = &(Colors.contrast);
    config.paddingX = 10;
    btn_quit = widget_build(config, renderer, Fonts.body);
    widget_set_postion(btn_quit, btn_rand->rect->x + btn_rand->rect->w + 2, 5);

    // pattern widget list
    genlist_init(&patternWidgets);

    // load patterns
    genlist_init(&patterns);
    int loaded = pattern_load_patternlist("patterns", &patterns, PATTERN_SORT_TITLE);

    if(loaded < 0) {
        LOG_ERROR("init: Unable to load pattern list.");
        return;
    }

    init_pattern_widgets(5, 50, 5, 5);
}

/**
 * render this screen
 */
void screen_start_render() {
    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);

    widget_render(btn_rand, renderer, NULL);
    widget_render(btn_quit, renderer, NULL);

    render_pattern_widgets();
    SDL_RenderPresent(renderer);

    if(previewTexture != NULL) {
        SDL_Rect canvas = { previewX, previewY, 0, 0 };
        SDL_QueryTexture(previewTexture, NULL, NULL, &(canvas.w), &(canvas.h));
        SDL_RenderCopy(renderer, previewTexture, NULL, &canvas);
        SDL_RenderPresent(renderer);
    }

}

/**
 * manages events for this screen
 * @param e SDL Event from main()
 * @param world world pattern
 */
void screen_start_events(SDL_Event e, Pattern *world) {
    events_btn_rand(e, world);
    events_btn_quit(e, world);
    events_pattern_widgets(e, world);
}

/**
 * destroys this screen and components
 */
void screen_start_destroy() {
    widget_destroy(btn_quit);
    widget_destroy(btn_rand);
    //todo widgets free
    for (int i = 0; i < patternWidgets.length; i++) {
        Widget *widget = (Widget *) genlist_get(&patternWidgets, i);
        widget_destroy(widget);
    }
    genlist_free(&patternWidgets);
    pattern_free_patternlist(&patterns);
    genlist_free(&patterns);
}
