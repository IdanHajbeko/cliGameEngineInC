#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct snakeNode {
    int x, y;
    struct snakeNode *before;
};

void show_score(int score);
int is_opposite(int dir1, int dir2);
int move_snake(struct snakeNode *head, struct object *snakePixel, int direction, int step, int *score, struct object * apple);
void free_snake(struct snakeNode *head);
void move_apple(struct object * apple);

int is_opposite(int dir1, int dir2) {
    return (dir1 == 1 && dir2 == 2) || (dir1 == 2 && dir2 == 1) ||
           (dir1 == 3 && dir2 == 4) || (dir1 == 4 && dir2 == 3);
}

int move_snake(struct snakeNode *head, struct object *snakePixel, int direction, int step, int *score, struct object * apple) {
    if (direction > 4 || direction <= 0) return 0;

    int dx = 0, dy = 0;
    switch (direction) {
        case 1: dy = -step; break; // up
        case 2: dy = step; break;  // down
        case 3: dx = step; break;  // right
        case 4: dx = -step; break; // left
    }

    int newX = head->x + dx;
    int newY = head->y + dy;

    if (newX < 0 || newX >= screenSizeX || newY < 0 || newY >= screenSizeY)
        return 0;

    struct snakeNode *current = head;
    while (current->before) current = current->before;
    clear_object_on_point(snakePixel, ' ', "\e[47m", current->x, current->y);

    current = head;
    int prevX = head->x, prevY = head->y;
    while (current->before) {
        int tmpX = current->before->x;
        int tmpY = current->before->y;
        current->before->x = prevX;
        current->before->y = prevY;
        prevX = tmpX;
        prevY = tmpY;
        current = current->before;
    }

    if (!strcmp(screen[newY][newX].style, "\e[42m")) return 0;
    head->x = newX;
    head->y = newY;
    if (check_style_collision(snakePixel, "\e[41m", head->x, head->y)){
        (*score)++;
        move_apple(apple);
        struct snakeNode *last = head;
        while (last->before) last = last->before;

        struct snakeNode *newNode = malloc(sizeof(struct snakeNode));
        newNode->x = last->x;
        newNode->y = last->y;
        newNode->before = NULL;
        last->before = newNode;

    }
    put_object_on_point(snakePixel, head->x, head->y);
    return 1;
}

void free_snake(struct snakeNode *head) {
    struct snakeNode *current = head->before;
    while (current) {
        struct snakeNode *tmp = current;
        current = current->before;
        free(tmp);
    }
}

void move_apple(struct object * apple){
    int x, y;
    while(1){
        x = rand() % (screenSizeX - 3);
        y = rand() % (screenSizeY - 1);
        int check = 1;
        for (int i = 0; i < 2; i++){
            for (int j = 0; j < 4; j++){
                if (screen[i+y][j+x].style == "\e[42m") check=0;
            }
        }
        if (check == 1) break;
    }
    move_object(apple, x, y, ' ', "\e[47m", 0);
}

void show_score(int score) {
    char buffer[12];
    int i = 0;

    if (score == 0) {
        screen[1][screenSizeX / 2].ch = '0';
        screen[1][screenSizeX / 2].style = "\e[0;33m";
        return;
    }

    while (score > 0 && i < 11) {
        buffer[i++] = (score % 10) + '0';
        score /= 10;
    }
    for (int j = 0; j < i; ++j) {
        screen[1][screenSizeX / 2 + j] = (struct pixel){.ch = buffer[i - j - 1], .style = "\e[0;33m"};
    }
}

int main() {
    get_terminal_size(&screenSizeY, &screenSizeX);
    init_screen(' ', "\e[47m");
    startKeyListener();

    struct object snakePixel;
    init_object(&snakePixel, 0, 0, 4, 2);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 4; ++j) {
            snakePixel.space[i][j].ch = ' ';
            snakePixel.space[i][j].style = "\e[42m";
        }
    }

    int direction = 3;
    int currentDirection = direction;

    struct snakeNode head;
    head.x = screenSizeX / 2;
    head.y = screenSizeY / 2;
    head.before = NULL;

    struct object apple;
    init_object(&apple, 0, 0, 4, 2);
    for (int i = 0; i < 2; i++){
        for (int j = 0; j <4; j++){
            apple.space[i][j].ch = ' ';
            apple.space[i][j].style = "\e[41m";
        }
    }
    move_apple(&apple);
    int step = 1, score = 0;
    while (1) {
        int key = last_key;
        last_key = 999;

        if (key == KEY_ESC || key == 'q') break;

        switch (key) {
            case 'w': case KEY_UP: if (!is_opposite(direction, 1)) direction = 1; break;
            case 's': case KEY_DOWN: if (!is_opposite(direction, 2)) direction = 2; break;
            case 'd': case KEY_RIGHT: if (!is_opposite(direction, 3)) direction = 3; break;
            case 'a': case KEY_LEFT: if (!is_opposite(direction, 4)) direction = 4; break;
        }
        if (direction == 1 || direction == 2) step = 2;
        else step = 4;
        if (!move_snake(&head, &snakePixel, direction, step, &score, &apple)) break;
        show_score(score);
        print_screen();
        sleep_ms((100-score >=
             25) ? 100-score : 25);
    }

    printf("\e[?25h");
    printf("press v to save the screen");
    fflush(stdout);
    sleep_ms(2500);
    if (last_key == 'v') save_screen("screenShot");
    free_object(&snakePixel);
    free_snake(&head);
    free_object(&apple);
    free_screen();

    return 0;
}
