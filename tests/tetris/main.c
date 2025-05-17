#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

struct block{
    short direction;
    struct object obj;
};
struct object block1, block2, block3, block4, block5, block6, block7;
void init_templates(char *backgroundColor);
void init_on_screen_block(struct object * template, struct block * blc);
void init_screen_base_ui();
struct object * get_random_template();
void move_block_down(struct block *currentBlock);
void move_right_left(struct block * currentBlock, int direction);
void check_for_lines(int y, int y2);
void move_all_line_down(short y);

void init_templates(char *backgroundColor) {
    init_object(&block1, 0, 0, 16, 2);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 16; j++) {
            block1.space[i][j].ch = ' ';
            block1.space[i][j].id = 1;
        }

    init_object(&block2, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block2.space[i][j].ch = ' ';
            block2.space[i][j].id = 0;
            block2.space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            for (int j = 0; j < 4; j++) {
                block2.space[i][j].id = 1;
            }
        } else {
            for (int j = 0; j < 12; j++) {
                block2.space[i][j].id = 1;
            }
        }
    }

    init_object(&block3, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block3.space[i][j].ch = ' ';
            block3.space[i][j].id = 0;
            block3.space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            for (int j = 8; j < 12; j++) {
                block3.space[i][j].id = 1;
            }
        } else {
            for (int j = 0; j < 12; j++) {
                block3.space[i][j].id = 1;
            }
        }
    }

    init_object(&block4, 0, 0, 8, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 8; j++) {
            block4.space[i][j].ch = ' ';
            block4.space[i][j].id = 1;
        }

    init_object(&block5, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block5.space[i][j].ch = ' ';
            block5.space[i][j].id = 0;
            block5.space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 12; j++) {
            if ((i < 2 && j > 3) || (i >= 2 && j < 8)) {
                block5.space[i][j].id = 1;
            }
        }
    }

    init_object(&block6, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block6.space[i][j].ch = ' ';
            block6.space[i][j].id = 0;
            block6.space[i][j].style=strdup(backgroundColor);
        }
    for (int i = 0; i < 4; i++) {
        if (i < 2) {
            for (int j = 4; j < 8; j++) {
                block6.space[i][j].id = 1;
            }
        } else {
            for (int j = 0; j < 12; j++) {
                block6.space[i][j].id = 1;
            }
        }
    }

    init_object(&block7, 0, 0, 12, 4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 12; j++) {
            block7.space[i][j].ch = ' ';
            if ((i < 2 && j < 8) || (i >= 2 && j > 3)) block7.space[i][j].id = 1;
            else block7.space[i][j].id = 0;
            block7.space[i][j].style=strdup(backgroundColor);
        }
}

void init_on_screen_block(struct object *template, struct block *blc) {
    init_object(&blc->obj, screenSizeX/2-8, screenSizeY/2-20, screenSizeX/2-8+template->x2, screenSizeY/2-20 + template->y2);

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
            screen[i][j].id = 0;
        }
    }
}

struct object * get_random_template(){
    switch(rand() %7 +1){
        case 1: return &block1; break;
        case 2: return &block2; break;
        case 3: return &block3; break;
        case 4: return &block4; break;
        case 5: return &block5; break;
        case 6: return &block6; break;
        case 7: return &block7; break;
    }
}

void move_block_down(struct block *currentBlock) {
    if (currentBlock->obj.y2 >= screenSizeY/2+20) {
        check_for_lines(currentBlock->obj.y, currentBlock->obj.y2);
        init_on_screen_block(get_random_template(), currentBlock);
        put_object(&currentBlock->obj);
        return;
    }

    for (int i = currentBlock->obj.x; i < currentBlock->obj.x2 ; i++) {
        if (screen[currentBlock->obj.y2][i].id){
            if (screen[currentBlock->obj.y2-1][i].id){
                check_for_lines(currentBlock->obj.y, currentBlock->obj.y2);
                init_on_screen_block(get_random_template(), currentBlock);
                put_object(&currentBlock->obj);
                return;
            }
            else{
                move_object_with_no_id(&currentBlock->obj, currentBlock->obj.x, currentBlock->obj.y + 2, 0, ' ', "\e[40m", 0);
                return;
            }
        }
    }    
    move_object(&currentBlock->obj, currentBlock->obj.x, currentBlock->obj.y + 1, ' ', "\e[40m", 0);
}

void move_right_left(struct block * currentBlock, int direction){ // 0 - left 1 - right
    if (direction){
        if (currentBlock->obj.x2 + 4 > screenSizeX/2+20) return;
        for (int i = currentBlock->obj.y; i < currentBlock->obj.y2; i++) if (screen[i][currentBlock->obj.x2+3].id) return;
        move_object_with_no_id(&currentBlock->obj, currentBlock->obj.x + 4, currentBlock->obj.y, 0, ' ', "\e[40m", 0);
    } 
    else if (!direction){
        if (currentBlock->obj.x - 4 < screenSizeX/2-20) return;
        for (int i = currentBlock->obj.y; i < currentBlock->obj.y2; i++) if (screen[i][currentBlock->obj.x-4].id) return;
        move_object_with_no_id(&currentBlock->obj, currentBlock->obj.x - 4, currentBlock->obj.y, 0, ' ', "\e[40m", 0);
    }
}

void check_for_lines(int y, int y2){
    for(int i = y; i < y2; i++){
        short allGood = 1;
        for (int j = screenSizeX/2-20; j < screenSizeX/2+20; j++)
            if (screen[i][j].id == 0 ) allGood = 0;
        if (allGood){
            for (int j = screenSizeX/2-20; j < screenSizeX/2+20; j++) {
                screen[i][j].id = 0;
                screen[i][j].style = "\e[40m";
                sleep_ms(1);
                print_screen();
            }
            move_all_line_down(i);
        }
    }
}

void move_all_line_down(short y){
    for (int i = y; i > screenSizeY/2-20; i--){
        for (int j = screenSizeX/2-20; j < screenSizeX/2+20; j++){
            screen[i][j].style = screen[i-1][j].style;
            screen[i][j].id = screen[i-1][j].id;
        }
    }
}

int main() {
    srand(time(0));
    get_terminal_size(&screenSizeY, &screenSizeX);
    init_screen(' ', "\e[47m");
    startKeyListener();

    init_templates("\e[40m");


    int time = 0;
    struct block *currentBlock = malloc(sizeof(struct block));
    init_screen_base_ui();
    init_on_screen_block(get_random_template(), currentBlock);
    put_object(&currentBlock->obj);
    while(1){
        int key = last_key;
        last_key = 999;
        if (key == KEY_ESC || key == 'q') break;
        if (key == KEY_DOWN) move_block_down(currentBlock);
        if (key == KEY_RIGHT) move_right_left(currentBlock, 1);
        if (key == KEY_LEFT) move_right_left(currentBlock, 0);

        if (time >= 50){
            move_block_down(currentBlock);
            time = 0;
        }
        put_object_on_point(&currentBlock->obj, 10, 10);
        print_screen();
        sleep_ms(16);
        time += 16;
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
    free_object(&currentBlock->obj);

    free_screen();
    return 0;
}
