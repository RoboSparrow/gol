#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <malloc.h>

#include "state.h"
#include "renderer.h"
#include "widgets.h"
#include "pattern.h"
#include "gol.h"

extern SDL_Renderer *renderer;
extern SdlColors Colors;
extern SdlFonts Fonts;
extern RendererInfo rendererInfo;

Widget _btn_rand = { WSTATE_DEFAULT, "Random game" };
Widget *btn_rand =  &_btn_rand;

Widget _btn_quit = { WSTATE_DEFAULT, "Quit Game" };
Widget *btn_quit =  &_btn_quit;

GenList patterns;
GenList patternWidgets;

////
// control buttons
////

/**
 * events for "enter" button
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void events_btn_rand(SDL_Event e, GlobalState *App, Pattern *world) {
    widgets_btn_event(btn_rand, e);

    switch(btn_rand->state) {
        case WSTATE_RELEASED:
            printf("Random game...\n");
            gol_clear_data(world->data);
            gol_random(world);
            SDL_Delay(50);
            App->screen = SDL_SCREEN_WORLD;
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
void events_btn_quit(SDL_Event e, GlobalState *App, Pattern *world) {
    widgets_btn_event(btn_quit, e);

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
void init_pattern_widgets(int startX, int startY, int xMargin, int yMargin) {

    int x = startX;
    int y = startY;

    if(!patterns.length) {
        return;
    }
    Widget *widget;
    Pattern *pattern;
    for (int i = 0; i < patterns.length; i++) {
        pattern = (Pattern *) genlist_get(&patterns, i);

        widget = malloc(sizeof(Widget));

        widget->state = WSTATE_DEFAULT;
        widget->text = malloc(strlen(pattern->title));
        // todo check alloc
        strcpy(widget->text, pattern->title);
        widget->text_color = Colors.text;
        widget->bg_color = Colors.bg;
        widget->border_color = Colors.borders;

        widgets_button_init(widget, renderer, Fonts.body, x + xMargin, y);
        genlist_push(&patternWidgets, widget);

        y += (yMargin + widget->rect.h);
    }
}

/**
 * renders pattern widgets
 */
void render_pattern_widgets() {

    if(!patternWidgets.length) {
        return;
    }

    Widget *widget;
    for(int i = 0; i < patternWidgets.length; i++) {
        widget = (Widget *) genlist_get(&patternWidgets, i);
        // widgets_print_widgets(widget);
        widgets_button_render(widget, renderer);
    }
}

/**
 * events for pattern widgets
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void events_pattern_widgets(SDL_Event e, GlobalState *App, Pattern *world)  {

    if(!patternWidgets.length) {
        return;
    }

    Widget *widget;
    Pattern *pattern;

    for(int i = 0; i < patternWidgets.length; i++) {
        widget = (Widget *) genlist_get(&patternWidgets, i);
        pattern = (Pattern *) genlist_get(&patterns, i);

        // default events
        widgets_btn_event(widget, e);

        // actions
        switch(widget->state) {
            case WSTATE_RELEASED:

                printf("action %s: %s..\n", widget->text, pattern->file);
                gol_clear_data(world->data);
                int merged = pattern_load_file_and_merge(pattern->file, world, world->cols / 2, world->rows / 2, PATTERN_CENTER);
                //// TODO screen message!
                EXIT_MINUS(merged, "main: could not load pattern file\n");
                App->screen = SDL_SCREEN_WORLD;

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
void screen_start_init() {

    // control buttons
    btn_rand->text_color = Colors.bg;
    btn_rand->bg_color = Colors.text;
    btn_rand->border_color = Colors.borders;
    widgets_button_init(btn_rand, renderer, Fonts.body, 5, 5);

    btn_quit->text_color = Colors.bg;
    btn_quit->bg_color = Colors.text;
    btn_quit->border_color = Colors.borders;
    widgets_button_init(btn_quit, renderer, Fonts.body, 5 + btn_rand->rect.x + btn_rand->rect.w , 5);

    // pattern widget list
    genlist_init(&patternWidgets);

    // load patterns
    genlist_init(&patterns);
    int loaded = pattern_load_patternlist("patterns", &patterns);

    if(loaded < 0) {
        LOG_ERROR("init: Unable to load pattern list.");
        return;
    }

    init_pattern_widgets(0, 50, 5, 5);
}

/**
 * render this screen
 */
void screen_start_render() {
    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);

    widgets_button_render(btn_rand, renderer);
    widgets_button_render(btn_quit, renderer);

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
    widgets_button_destroy(btn_quit);
    widgets_button_destroy(btn_rand);
    //todo widgets free
    for (int i = 0; i < patternWidgets.length; i++) {
        Widget *widget = (Widget *) genlist_get(&patternWidgets, i);
        if(widget->text != NULL) {
            free(widget->text);
        }
        widgets_button_destroy(widget);
    }
    genlist_free(&patternWidgets);

    pattern_free_patternlist(&patterns);
    genlist_free(&patterns);
}
