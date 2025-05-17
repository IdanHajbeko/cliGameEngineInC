#include "engine.h"
#include <stdio.h>

int main() {
    get_terminal_size(&screenSizeY, &screenSizeX);
    init_screen(' ', "\e[43m");
    startKeyListener();
    
    struct object bob;
    init_object(&bob, 0, 0, 9, 4);
    for (int i = 0; i < bob.y2 - bob.y; i++) {
        for (int j = 0; j < bob.x2 - bob.x; j++) {
            bob.space[i][j].ch = ' ';
            bob.space[i][j].style = "\e[41m";
        }
    }
    bob.space[1][1].style = "\e[44m";
    bob.space[1][7].style = "\e[44m";
    bob.space[3][2].style = "\e[45m";
    bob.space[3][3].style = "\e[45m";
    bob.space[3][4].style = "\e[45m";
    bob.space[3][5].style = "\e[45m";
    bob.space[3][6].style = "\e[45m";

    put_object(&bob);
    print_screen();

    int speed = 1;
    while (1) {
        int key = last_key;
        last_key = 999;

        if (key == KEY_ESC || key == 'q') break;
        if (key >= '1' && key <= '9') speed = key - '0';
        switch (key) {
            case KEY_UP: if (!move_object(&bob, bob.x, bob.y-speed, ' ', "\e[5;45m", 0)) putchar('\a'); break;
            case KEY_DOWN: if (!move_object(&bob, bob.x, bob.y+speed, ' ', "\e[5;45m", 0)) putchar('\a'); break;
            case KEY_RIGHT: if (!move_object(&bob, bob.x+speed, bob.y, ' ', "\e[5;45m", 0)) putchar('\a'); break;
            case KEY_LEFT: if (!move_object(&bob, bob.x-speed, bob.y, ' ', "\e[5;45m", 0)) putchar('\a'); break;
            case KEY_SPACE:
                for (int i = 0; i< screenSizeY; ++i){
                    for (int j = 0; j<screenSizeX; ++j){
                        screen[i][j].style = "\e[47m";
                    }
                }
                put_object(&bob);
                break;
            case 's': save_screen("output.txt"); break;
        }


        print_screen();
        sleep_ms(16);
    }

    printf("\e[?25h");
    free_object(&bob);
    free_screen();
    return 0;
}