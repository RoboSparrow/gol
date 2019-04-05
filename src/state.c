#include <SDL2/SDL.h>

#include "state.h"

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

