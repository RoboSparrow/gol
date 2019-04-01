#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "state.h"
#include "renderer.h"
#include "widgets.h"
#include "pattern.h"

extern SDL_Renderer *renderer;
extern TTF_Font *font;
extern SdlColors Colors;
extern RendererInfo rendererInfo;

Widget _btn_enter = { WSTATE_DEFAULT, "Enter game" };
Widget *btn_enter =  &_btn_enter;

Widget _btn_quit = { WSTATE_DEFAULT, "Quit Game" };
Widget *btn_quit =  &_btn_quit;

PatternList _patterns = { 0, NULL };
PatternList *patterns = &_patterns;

/**
 * actions for "enter" button
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void btn_enter_actions(SDL_Event e, GlobalState *App, Pattern *world) {
    switch(btn_enter->state) {
        case WSTATE_ACTIVE:
            printf("Enter world screen..\n");
            SDL_Delay(250);
            App->screen = SDL_SCREEN_WORLD;
            return;
        default:
            return;
    }
}

/**
 * actions for "quit" button
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void btn_quit_actions(SDL_Event e, GlobalState *App, Pattern *world) {
    switch(btn_quit->state) {
        case WSTATE_ACTIVE:
            printf("Quitting app..\n");
            SDL_Delay(250);
            App->state = APP_STATE_QUIT;
            return;
        default:
            return;
    }
}

/**
 * initialize this screen
 * @param world world pattern
 */
void screen_start_init() {
    btn_enter->text_color = Colors.text;
    btn_enter->bg_color = Colors.bg;
    widgets_button_init(btn_enter, renderer, font, 5, 5);

    btn_quit->text_color = Colors.text;
    btn_quit->bg_color = Colors.bg;
    widgets_button_init(btn_quit, renderer, font, 5 + btn_enter->rect.x + btn_enter->rect.w , 5);

    int loaded = pattern_load_patternlist("patterns", patterns);
  printf("-----fdsfsf\n");
    if(loaded < 0) {
        LOG_ERROR("init: Unable to load pattern list.");
    }

}

/**
 * render this screen
 */
void screen_start_render() {
    renderer_set_color(Colors.bg);
    SDL_RenderClear(renderer);

    widgets_button_render(btn_enter, renderer);
    widgets_button_render(btn_quit, renderer);

    if(patterns->len) {
        for(int i = 0; i < patterns->len; i++) {
            printf("%d) %s (%dx%d)\n", i + 1, patterns->patterns[i]->title, patterns->patterns[i]->cols, patterns->patterns[i]->rows);
            printf("    * %s\n", patterns->patterns[i]->description);
        }
    } else {
        printf(" === ffff\n");
    }

    SDL_RenderPresent(renderer);
}

/**
 * manages events for this screen
 * @param e SDL Event from main()
 * @param App global App state from main(). The state may be updated
 * @param world world pattern
 */
void screen_start_events(SDL_Event e, GlobalState *App, Pattern *world) {
    widgets_btn_event(btn_enter, e);
    widgets_btn_event(btn_quit, e);

    btn_enter_actions(e, App, world);
    btn_quit_actions(e, App, world);
}

/**
 * destroys this screen and components
 */
void screen_start_destroy() {
    widgets_button_destroy(btn_enter);
}
