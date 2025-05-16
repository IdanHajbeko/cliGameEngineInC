#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define POLE_WIDTH 14
#define COLOR_BLUE "\e[44m"
#define COLOR_GREEN "\e[42m"

void create_new_pole(struct object * pole);
void move_all_poles(struct object *poles, int *alive, int *score, int flappyX);
void show_score(int score);
void die(short * run, struct object * flappy);

void create_new_pole(struct object *pole) {
    init_object(pole, screenSizeX - POLE_WIDTH, 0, screenSizeX, screenSizeY);

    int minGapSize = 12;
    int maxGapSize = 17;
    int gapSize = rand() % (maxGapSize - minGapSize + 1) + minGapSize;
    int gapStart = rand() % (screenSizeY - gapSize - 2) + 1;

    for (int y = 0; y < screenSizeY; ++y) {
        int inGap = (y >= gapStart && y < gapStart + gapSize);

        for (int x = 0; x < POLE_WIDTH; ++x) {
            pole->space[y][x].ch = ' ';

            if (inGap) {
                pole->space[y][x].style = COLOR_BLUE;
            } else if ((x == 0 || x == POLE_WIDTH - 1) && (y == gapStart - 1 || y == gapStart + gapSize)) {
                pole->space[y][x].style = COLOR_GREEN;
            } else {
                if (x == 0 || x == POLE_WIDTH - 1) {
                    pole->space[y][x].style = COLOR_BLUE;
                } else {
                    pole->space[y][x].style = COLOR_GREEN;
                }
            }
        }
    }
}

void move_all_poles(struct object *poles, int *alive, int *score, int flappyX) {
    int i = 0;
    while (i < *alive) {
        if (poles[i].x + POLE_WIDTH == flappyX) {
            (*score)++;
            putchar('\a');
        }

        if (!move_object(&poles[i], poles[i].x - 1, poles[i].y, ' ', COLOR_BLUE)) {
            clear_object(&poles[i], ' ', COLOR_BLUE);
            free_object(&poles[i]);
            for (int j = i; j < *alive - 1; ++j) {
                poles[j] = poles[j + 1];
            }
            (*alive)--;
        } else {
            i++;
        }
    }
}

void paint_flappy(struct object * flappy, char * style){
    for (int i = 0; i < flappy->y2 - flappy->y; ++i) {
        for (int j = 0; j < flappy->x2 - flappy->x; ++j) {
            flappy->space[i][j].ch = ' ';
            if (i == 0 && j < 5){
                flappy->space[i][j].style = COLOR_BLUE;
                flappy->space[i][j].id = 999;
            }
            else
                flappy->space[i][j].style = style;
        }
    }

    flappy->space[1][0].style = COLOR_BLUE;
    flappy->space[1][1].style = COLOR_BLUE;
    flappy->space[1][0].id = 999;
    flappy->space[1][1].id = 999;
    flappy->space[1][12].style = "\e[40m";
    for (int i = 0; i < 3; i++) flappy->space[3][10 + i].style = "\e[41m";
}

void show_score(int score) {
    char buffer[12];
    int i = 0;

    if (score == 0) {
        screen[1][screenSizeX / 2].ch = '0';
        return;
    }

    while (score > 0 && i < 11) {
        buffer[i++] = (score % 10) + '0';
        score /= 10;
    }
    for (int j = 0; j < i; ++j) {
        screen[1][screenSizeX / 2 + j] = (struct pixel){.ch = buffer[i - j - 1], .style = screen[1][screenSizeX / 2 + j].style};
    }
}

void die(short * run, struct object * flappy){
    paint_flappy(flappy, "\e[41m"); 
    put_object(flappy);             
    print_screen();                 
    sleep_ms(500);                  
    (*run) = 0;                     
}

int main() {
    get_terminal_size(&screenSizeY, &screenSizeX);
    init_screen(' ', COLOR_BLUE);
    startKeyListener();

    struct object flappy;
    init_object(&flappy, screenSizeX / 10, screenSizeY / 2, screenSizeX / 10 + 14, screenSizeY / 2 + 5);
    paint_flappy(&flappy, "\e[43m");
    put_object(&flappy);

    int time = 0, time2 = 0;
    struct object poles[10];
    int alivePoles = 0;
    short run = 1;
    int score = 0;
    while (run) {
        int key = last_key;
        last_key = 999;
        if (key == KEY_ESC || key == 'q') break;

        if (key == KEY_SPACE || key == 'w' || key == KEY_UP){
            if(check_style_collision(&flappy, COLOR_GREEN, flappy.x, flappy.y - 5)) {
                run = 0;
                paint_flappy(&flappy, "\e[41m");
            }
            move_object(&flappy, flappy.x, flappy.y - 5, ' ', COLOR_BLUE);
        }
        if ((time >= 50 - score/5 && 50 - score/5 > 25) || (50 - score/5 <= 25 && time/25>= 25)) {
            move_all_poles(poles, &alivePoles, &score, flappy.x);
            if(check_style_collision(&flappy, COLOR_GREEN, flappy.x, flappy.y + 1) || !move_object(&flappy, flappy.x, flappy.y + 1, ' ', COLOR_BLUE)) die(&run, &flappy);
            time = 0;
        }

        if (time2 >= rand() % (3000- 5000 + 1) + 3000) {
            time2 = 0;
            if (alivePoles < 9) {
                create_new_pole(&poles[alivePoles]);
                alivePoles++;
            }
        }
        show_score(score);
        print_screen();
        sleep_ms(16);
        time += 16;
        time2 += 16;
    }

    printf("press s to save the screen");
    printf("\e[?25h");
    fflush(stdout);
    sleep_ms(2000);
    if (last_key == 's' || last_key == 'S') save_screen("screenShot");
    for (int i = 0; i < alivePoles; i++) 
        free_object(&poles[i]);
    free_object(&flappy);
    free_screen();
    return 0;
}