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

Widget *btn_rand = NULL;
Widget *btn_quit = NULL;

GenList patterns;
GenList patternWidgets;

int wid = 0; //todo static in widget func

////
// control buttons
////

/**
 * events for "enter" button
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
static void events_btn_rand(SDL_Event e, GlobalState *App, Pattern *world) {
    widget_event(btn_rand, e);

    switch(btn_rand->state) {
        case WSTATE_RELEASED:
            printf("Random game...\n");
            gol_clear_data(world->data);
            gol_random(world);
            SDL_Delay(50);
            game_start(App, world);
            return;
        default:
            return;
    }
}

/**
 * events for "quit" button
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
static void events_btn_quit(SDL_Event e, GlobalState *App, Pattern *world) {
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

    Pattern *pattern;
    int height;
    for (int i = 0; i < patterns.length; i++) {
        pattern = (Pattern *) genlist_get(&patterns, i);

        WidgetConfig config = widget_configure(wid++, WTYPE_BUTTON, pattern->title);
        config.fgColor = &(Colors.text);
        config.bgColor = &(Colors.bg);
        config.bgColorH = &(Colors.contrast);
        Widget *widget = widget_build(config, renderer, Fonts.body);
        widget_set_postion(widget, x, y);
        genlist_push(&patternWidgets, widget);

        height = widget->rect->h + yMargin;
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

    Widget *widget;
    for(int i = 0; i < patternWidgets.length; i++) {
        widget = (Widget *) genlist_get(&patternWidgets, i);
        // widgets_print_widgets(widget);
        widget_render(widget, renderer, NULL);
    }
}

/**
 * events for pattern widgets
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
static void events_pattern_widgets(SDL_Event e, GlobalState *App, Pattern *world)  {
    if(!patternWidgets.length) {
        return;
    }

    Widget *widget;
    Pattern *pattern;

    for(int i = 0; i < patternWidgets.length; i++) {
        widget = (Widget *) genlist_get(&patternWidgets, i);
        pattern = (Pattern *) genlist_get(&patterns, i);

        // default events
        widget_event(widget, e);

        // actions
        switch(widget->state) {
            case WSTATE_RELEASED:
                printf("action widget (id:%d): %s..\n", widget->id, pattern->file);
                int merged = game_merge(world, pattern->file, world->cols / 2, world->rows / 2, PATTERN_CENTER);
                EXIT_MINUS(merged, "main: could not load pattern file"); // TODO screen message!
                int started = game_start(App, world);
                EXIT_MINUS(started, "Could not restart game"); // TODO screen message!
            break;

            default:
            break;
        }

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
    int loaded = pattern_load_patternlist("patterns", &patterns);

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
}

/**
 * manages events for this screen
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void screen_start_events(SDL_Event e, GlobalState *App, Pattern *world) {
    events_btn_rand(e, App, world);
    events_btn_quit(e, App, world);
    events_pattern_widgets(e, App, world);
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
