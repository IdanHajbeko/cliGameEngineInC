#include "engine.h"
#include <stdio.h>

int main(){
    get_terminal_size(&screenSizeY, &screenSizeX);
    if (screenSizeY > 30 && screenSizeX > 100){
        printf("zoom in your terminal a lot\n");
        fflush(stdout);
        sleep_ms(3000);
        get_terminal_size(&screenSizeY, &screenSizeX);
    }
    while (screenSizeY > 30 && screenSizeX > 100)
    {
        printf("zoom more!!\n");
        fflush(stdout);
        sleep_ms(1000);
        get_terminal_size(&screenSizeY, &screenSizeX);
    }

        

    return 0;
}