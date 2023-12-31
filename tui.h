#ifndef TUI_H
#define TUI_H
#include <stdlib.h> 

extern const unsigned int MAX_CHARS_PER_CELL;

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct cell {
    wchar_t *content;
    unsigned int width;
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
    size_t rows;
    size_t cols;
    buffer buf;
    struct str_buffer str_buf;
    wchar_t *debug;
};

struct print_options {
    unsigned int x;
    unsigned int y;
    struct color *fg_color;
    struct color *bg_color;
};

struct tui *init_tui(size_t rows, size_t cols);
void free_tui(struct tui *tui);
void refresh(struct tui *tui);
void clear(struct tui *tui);
int print_tui(struct tui *tui, struct print_options opt, wchar_t *str);
int print_tui_len(struct tui *tui, struct print_options opt, wchar_t *str, unsigned int len);
void debug_tui(struct tui *tui, wchar_t *str);

#endif
