#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct block{
    short direction;
    struct object obj;
};

void init_templates(struct object *block1, struct object *block2, struct object *block3,
                 struct object *block4, struct object *block5, struct object *block6,
                 struct object *block7, char *backgroundColor);
void init_on_screen_block(struct object * template, struct block * blc);
void init_screen_base_ui();


void init_templates(struct object *block1, struct object *block2, struct object *block3,
                 struct object *block4, struct object *block5, struct object *block6,
                 struct object *block7, char *backgroundColor) {
    init_object(block1, 0, 0, 16, 2);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 16; j++) {
            block1->space[i][j].ch = ' ';
            block1->space[i][j].id = 1;
        }

    init_object(block2, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block2->space[i][j].ch = ' ';
            block2->space[i][j].id = 0;
            block2->space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            for (int j = 0; j < 4; j++) {
                block2->space[i][j].id = 1;
            }
        } else {
            for (int j = 0; j < 12; j++) {
                block2->space[i][j].id = 1;
            }
        }
    }

    init_object(block3, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block3->space[i][j].ch = ' ';
            block3->space[i][j].id = 0;
            block3->space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            for (int j = 7; j < 12; j++) {
                block3->space[i][j].id = 1;
            }
        } else {
            for (int j = 0; j < 12; j++) {
                block3->space[i][j].id = 1;
            }
        }
    }

    init_object(block4, 0, 0, 8, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 8; j++) {
            block4->space[i][j].ch = ' ';
            block4->space[i][j].id = 1;
        }

    init_object(block5, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block5->space[i][j].ch = ' ';
            block5->space[i][j].id = 0;
            block5->space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 12; j++) {
            if ((i < 2 && j > 3) || (i >= 2 && j < 8)) {
                block5->space[i][j].id = 1;
            }
        }
    }

    init_object(block6, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block6->space[i][j].ch = ' ';
            block6->space[i][j].id = 0;
            block6->space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            for (int j = 4; j < 8; j++) {
                block6->space[i][j].id = 1;
            }
        } else {
            for (int j = 0; j < 12; j++) {
                block6->space[i][j].id = 1;
            }
        }
    }

    init_object(block7, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block7->space[i][j].ch = ' ';
            block7->space[i][j].id = 0;
            block7->space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 12; j++) {
            if ((i < 2 && j < 8) || (i >= 2 && j > 3)) {
                block7->space[i][j].id = 1;
            }
        }
    }
}

void init_on_screen_block(struct object *template, struct block *blc) {
    init_object(&blc->obj, screenSizeX/2-template->x2/2, screenSizeY/2-20, screenSizeX/2+template->x2/2, screenSizeY/2-20 + template->y2);

    for (int i = 0; i < template->y2; ++i) {
        for (int j = 0; j < template->x2; j++) {
            blc->obj.space[i][j].ch = template->space[i][j].ch;
            blc->obj.space[i][j].id = template->space[i][j].id;

            if (blc->obj.space[i][j].style != NULL) {
                free(blc->obj.space[i][j].style);
            }

            blc->obj.space[i][j].style = template->space[i][j].style ? strdup(template->space[i][j].style) : NULL;
        }
    }

    char color[16];
    int color_num = rand() % 6 + 1;
    snprintf(color, sizeof(color), "\e[4%dm", color_num);

    int height = blc->obj.y2 - blc->obj.y;
    int width  = blc->obj.x2 - blc->obj.x;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (blc->obj.space[i][j].id == 1) {
                free(blc->obj.space[i][j].style);
                blc->obj.space[i][j].style = strdup(color);
            }
        }
    }
}


void init_screen_base_ui(){
    // scrren 10X20 tetris blocks
    for (int i = screenSizeY/2-20; i < screenSizeY/2+20; i++){
        for (int j = screenSizeX/2-20; j < screenSizeX/2+20; j++){
            screen[i][j].ch = ' ';
            screen[i][j].style = "\e[40m";
            screen[i][j].id = 3;
        }
    }
}



int main() {
    srand(time(0));
    get_terminal_size(&screenSizeY, &screenSizeX);
    init_screen(' ', "\e[47m");
    startKeyListener();

    struct object block1, block2, block3, block4, block5, block6, block7;
    init_templates(&block1, &block2, &block3, &block4, &block5, &block6, &block7, "\e[40    m");

    struct block onBlock1, onBlock2;
    init_on_screen_block(&block5, &onBlock1);
    init_on_screen_block(&block1, &onBlock2);
    init_screen_base_ui();
    put_object(&onBlock1.obj);

    while(1){
        int key = last_key;
        last_key = 999;
        if (key == KEY_ESC || key == 'q') break;



        print_screen();
        sleep_ms(16);
    }



    printf("\e[?25h");
    fflush(stdout);

    free_object(&block1);
    free_object(&block2);
    free_object(&block3);
    free_object(&block4);
    free_object(&block5);
    free_object(&block6);
    free_object(&block7);
    free_object(&onBlock1.obj);
    free_object(&onBlock2.obj);

    free_screen();
    return 0;
}
