#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"

// global varibales
int screenSizeX;
int screenSizeY;
struct pixel **screen;
volatile int last_key = 0;

int getch() {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        int ch2 = _getch();
        switch (ch2) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 77: return KEY_RIGHT;
            case 75: return KEY_LEFT;
            case 83: return KEY_DELETE;
            default: return KEY_UNKNOWN;
        }
    } else if (ch == 27) return KEY_ESC;
    else if (ch == ' ') return KEY_SPACE;
    else if (ch == 8) return KEY_BACKSPACE;
    else if (ch == 9) return KEY_TAB;
    else if (ch == 13) return KEY_ENTER;
    return ch;

#else
    struct termios oldt, newt;
    char ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    if (ch == '\x1b') {
        if (kbhit_timeout(50)) {
            char seq1 = getch();
            if (seq1 == '[' && kbhit_timeout(50)) {
                char seq2 = getch();
                switch (seq2) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                    case '3':
                        if (kbhit_timeout(50) && getch() == '~')
                            return KEY_DELETE;
                        break;
                    default: return KEY_UNKNOWN;
                }
            } else return KEY_ESC_SEQ;
        } else return KEY_ESC;
    } else if (ch == ' ') return KEY_SPACE;
    else if (ch == 127 || ch == 8) return KEY_BACKSPACE;
    else if (ch == 9) return KEY_TAB;
    else if (ch == 10 || ch == 13) return KEY_ENTER;
    return ch;
#endif
}

#ifndef _WIN32
int kbhit_timeout(int timeout_ms) {
    struct timeval tv = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000
    };
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0;
}
#endif

#ifdef _WIN32
void key_listener_thread(void* arg) {
    while (1) {
        int k = getch();
        last_key = k;
    }
}
#else
void* key_listener_thread(void* arg) {
    while (1) {
        int k = getch();
        last_key = k;
    }
    return NULL;
}
#endif


void sleep_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
    }

void get_terminal_size(int *rows, int *cols) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *cols = 80;
        *rows = 25;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        *cols = w.ws_col;
        *rows = w.ws_row;
    } else {
        *cols = 80;
        *rows = 25;
    }
#endif
}

void print_screen(){
    printf("\x1b[H\e[?25l");
    for (int i = 0; i < screenSizeY; ++i){
        for (int j = 0; j < screenSizeX; ++j){
            if (screen[i][j].style == NULL)
                printf("\e[0m%c", screen[i][j].ch);
            else
                printf("%s%c\e[0m", screen[i][j].style, screen[i][j].ch);
        }
        if (i != screenSizeY - 1)
            putchar('\n');
    }
    fflush(stdout);
}

void init_screen(char ch, char * style) {
    screen = malloc(screenSizeY * sizeof(struct pixel *));
    for (int i = 0; i < screenSizeY; i++) {
        screen[i] = malloc(screenSizeX * sizeof(struct pixel));
        for (int j = 0; j < screenSizeX; j++) {
            screen[i][j].ch = ch;
            screen[i][j].style = style;
        }
    }
}

struct pixel ** init_space(int x, int y){
    struct pixel **space = malloc(y * sizeof(struct pixel *));
    for (int i = 0; i < y; i++){
        space[i] = malloc(screenSizeX * sizeof(struct pixel));
        for (int j = 0; j < x; j++){
            space[i][j].ch = -1;
            space[i][j].style = NULL;
        }
    }
    return space;
}

void free_screen() {
    for (int i = 0; i < screenSizeY; i++) {
        free(screen[i]);
    }
    free(screen);
}

void startKeyListener(){
#ifdef _WIN32
    _beginthread(key_listener_thread, 0, NULL);
#else
    pthread_t tid;
    pthread_create(&tid, NULL, key_listener_thread, NULL);
    pthread_detach(tid);
#endif
}

int put_object(struct object * obj){
    if (obj->y < 0 || obj->y2 > screenSizeY || obj->x < 0 || obj->x2 > screenSizeX) return 0;
    for (int i = 0; i < obj->y2 - obj->y; i++) {
        for (int j = 0; j < obj->x2 - obj->x; j++) {
            if ((i + obj->y < screenSizeY) && (j + obj->x < screenSizeX)) {
                screen[i + obj->y][j + obj->x] = obj->space[i][j];
            }
        }
    }
    return 1;
}

int clear_object(struct object * obj, char backgroundChar, char * backgroundStyle, int id){
    if (obj->y < 0 || obj->y2 > screenSizeY || obj->x < 0 || obj->x2 > screenSizeX) return 0;
    for (int i = 0; i < obj->y2 - obj->y; i++) {
        for (int j = 0; j < obj->x2 - obj->x; j++) {
            screen[i + obj->y][j + obj->x].ch = backgroundChar;
            screen[i + obj->y][j + obj->x].style = backgroundStyle;
            screen[i + obj->y][j + obj->x].id = id;
        }
    }
}


// todo - add a full background
int move_object(struct object * obj, int x, int y, char backgroundChar, char * backgroundStyle, int id) {
    int lenx = obj->x2 - obj->x;
    int leny = obj->y2 - obj->y;

    if (x < 0 || y < 0 || x + lenx > screenSizeX || y + leny > screenSizeY)
        return 0;

    clear_object(obj, backgroundChar, backgroundStyle, id);
    obj->x = x;
    obj->y = y;
    obj->x2 = x + lenx;
    obj->y2 = y + leny;

    put_object(obj);
    return 1;
}

void save_screen(char * fileName){
    FILE * file;
    file = fopen(fileName, "a");
    for (int i = 0; i < screenSizeY; ++i){
        for (int j = 0; j < screenSizeX; ++j){
            if (screen[i][j].style == NULL)
                fprintf(file, "\e[0m%c", screen[i][j].ch);
            else
                fprintf(file, "%s%c\e[0m", screen[i][j].style, screen[i][j].ch);
        }
        if (i != screenSizeY - 1) fprintf(file, "\n");
    }
    fclose(file); 
}

int init_object(struct object * obj, int x, int y, int x2 ,int y2){
    if (x >= x2 || y >= y2 || x < 0 || y < 0 || x2 > screenSizeX || y2 > screenSizeY) return 0;
    obj->x = x;
    obj->y = y;
    obj->x2 = x2;
    obj->y2 = y2;
    obj->space = init_space(x2 - x, y2 - y);

    return 1;
}

void free_object(struct object *obj) {
    if (!obj || !obj->space) return;

    int height = obj->y2 - obj->y;
    for (int i = 0; i < height; ++i) {
        free(obj->space[i]);
    }
    free(obj->space);
    obj->space = NULL;
}

int check_style_collision(struct object *obj1, char *style, int x, int y) {
    for (int i = 0; i < obj1->y2 - obj1->y; ++i) {
        if ( y + i < 0 ||  y + i >= screenSizeY) continue;

        for (int j = 0; j < obj1->x2 - obj1->x; ++j) {
            int screenX = x + j;
            if (obj1->space[i][j].id == 999) continue;
            if (screen[y + i][x + j].id == 999) continue;
            if (screen[y + i][x + j].style == style) return 1;
        }
    }

    return 0;
}


int check_char_collision(struct object *obj1, char ch, int x, int y) {
    if (x < 0 || y < 0 || x + obj1->x2 - obj1->x > screenSizeX || y + obj1->y2 - obj1->y > screenSizeY) return 0;

    for (int i = 0; i < obj1->y2 - obj1->y; ++i) {
        if (y + i < 0 ||y + i >= screenSizeY) continue;
        for (int j = 0; j < obj1->x2 - obj1->x; ++j) {
            if (x + j < 0 || x + j >= screenSizeX) continue;
            if (obj1->space[i][j].id == 999) continue;
            if (screen[y + i][x + j].id == 999) continue;
            if (screen[y + i][x + j].ch == ch) return 1;
        }
    }

    return 0;
}

int check_id_collision(struct object *obj1, int id, int x, int y) {
    if (x < 0 || y < 0 || x + obj1->x2 - obj1->x > screenSizeX || y + obj1->y2 - obj1->y > screenSizeY) return 0;

    for (int i = 0; i < obj1->y2 - obj1->y; ++i) {
        if (y + i < 0 || y + i >= screenSizeY) continue;
        for (int j = 0; j < obj1->x2 - obj1->x; ++j) {
            if (x + j < 0 || x + j >= screenSizeX) continue;
            if (screen[y + i][x + j].id == id) return 1;
        }
    }

    return 0;
}

int put_object_on_point(struct object * obj, int x, int y){
    if (y < 0 || obj->y2 - obj->y+ y > screenSizeY || x < 0 ||  obj->x2 - obj->x + x > screenSizeX) return 0;
    for (int i = 0; i < obj->y2 - obj->y; i++) {
        for (int j = 0; j < obj->x2 - obj->x; j++) {
            if ((i + y < screenSizeY) && (j + x < screenSizeX)) {
                screen[i + y][j + x] = obj->space[i][j];
            }
        }
    }
    return 1;
}

int clear_object_on_point(struct object * obj, char backgroundChar, char * backgroundStyle, int x, int y){
    if (y < 0 || obj->y2 - obj->y+ y > screenSizeY || x < 0 ||  obj->x2 - obj->x + x > screenSizeX) return 0;
    for (int i = 0; i < obj->y2 - obj->y; i++) {
        for (int j = 0; j < obj->x2 - obj->x; j++) {
            screen[i + y][j + x].ch = backgroundChar;
            screen[i + y][j + x].style = backgroundStyle;
        }
    }
}

int move_object_with_no_id(struct object * obj, int x, int y, int idToIgnore, char backgroundChar, char * backgroundStyle, int id) {
    int lenx = obj->x2 - obj->x;
    int leny = obj->y2 - obj->y;

    if (x < 0 || y < 0 || x + lenx > screenSizeX || y + leny > screenSizeY)
        return 0;

    clear_object(obj, backgroundChar, backgroundStyle, id);
    obj->x = x;
    obj->y = y;
    obj->x2 = x + lenx;
    obj->y2 = y + leny;

    put_object_with_no_id(obj, idToIgnore);
    return 1;
}

int put_object_with_no_id(struct object * obj, int idToIgnore){
    if (obj->y < 0 || obj->y2 > screenSizeY || obj->x < 0 || obj->x2 > screenSizeX) return 0;
    for (int i = 0; i < obj->y2 - obj->y; i++) {
        for (int j = 0; j < obj->x2 - obj->x; j++) {
            if ((i + obj->y < screenSizeY) && (j + obj->x < screenSizeX) && obj->space[i][j].id != idToIgnore) {
                screen[i + obj->y][j + obj->x] = obj->space[i][j];
            }
        }
    }
    return 1;
}