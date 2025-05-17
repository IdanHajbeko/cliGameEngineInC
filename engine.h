#ifndef ENGINE_H
#define ENGINE_H

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #include <process.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
    #include <sys/ioctl.h>
    #include <pthread.h>
#endif

// Key codes
#define KEY_UP         -1
#define KEY_DOWN       -2
#define KEY_RIGHT      -3
#define KEY_LEFT       -4
#define KEY_ESC        -5
#define KEY_SPACE      -6
#define KEY_BACKSPACE  -7
#define KEY_TAB        -8
#define KEY_ENTER      -9
#define KEY_DELETE     -10
#define KEY_ESC_SEQ    -400
#define KEY_UNKNOWN    -404

// structs
struct pixel{
    char ch;
    char *style;
    short id;
};

struct object{
    int x, y, x2, y2;
    struct pixel **space;
};

// global vars
extern int screenSizeX;
extern int screenSizeY;
extern struct pixel **screen;
extern volatile int last_key;

// function dec
int key();
int kbhit_timeout(int timeout_ms);
void print_screen();
void get_terminal_size(int *rows, int *cols);
void init_screen(char ch, char * style);
void free_screen();
void sleep_ms(int milliseconds);
void startKeyListener();
struct pixel ** init_space(int x, int y);
int put_object(struct object * obj);
int move_object(struct object * obj, int x, int y, char backgroundChar, char * backgroundStyle, int id);
int clear_object(struct object * obj, char backgroundChar, char * backgroundStyle, int id);
void save_screen(char * fileName);
int init_object(struct object * obj, int x, int y, int x2 ,int y2);
void free_object(struct object *obj);
int check_style_collision(struct object * obj1, char * style, int x, int y);
int check_char_collision(struct object * obj1, char ch, int x, int y);
int put_object_on_point(struct object * obj, int x, int y);
int clear_object_on_point(struct object * obj, char backgroundChar, char * backgroundStyle, int x, int y);
int check_id_collision(struct object *obj1, int id, int x, int y);
int move_object_with_no_id(struct object * obj, int x, int y, int idToIgnore, char backgroundChar, char * backgroundStyle, int id);
int put_object_with_no_id(struct object * obj, int idToIgnore);
#ifdef _WIN32
    void key_listener_thread(void* arg);
#else
    void* key_listener_thread(void* arg);
#endif

#endif
