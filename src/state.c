#include <SDL2/SDL.h>

#include "state.h"

/* define externals */
SdlColors Colors = {
    .bg = { 44, 74, 82 }, // #2C4A52
    .fg = { 244, 235, 219 }, // #F4EBDB
    .grid ={ 10, 49, 57 }, // bg_color darkened 10%
    .text ={ 134, 155, 151 }
};

SdlFonts Fonts; //populated by renderer
