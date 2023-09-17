#ifndef TUI_H
#define TUI_H
#include <stdlib.h> 

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct cell {
    wchar_t character;
    struct color fg_color;
    struct color bg_color;
};

typedef struct cell **buffer;

struct str_buffer {
    wchar_t *data;
    size_t capacity;
    size_t length;
};

struct tui {
    buffer buf;
    struct str_buffer str_buf;
};

struct tui *init_tui();
void free_tui(struct tui *tui);
void refresh(struct tui *tui);

#endif