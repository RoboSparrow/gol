#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "gol.h"
#include "paint.h"

int main()
{
    int width = 30;
    int height = 30;

    int size = width * height;
    int world[size];

    srand (time(NULL));

    gol_init(world, width, height);
    paint_init();
    usleep(100000);

    while (1){
        gol_update(world, width, height);
        usleep(100000);
    }

    return 0;
}
