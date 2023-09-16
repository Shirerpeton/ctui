#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>

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

void init_buffer(size_t rows, size_t cols, struct cell ***buffer);
void free_buffer(size_t rows, struct cell ***buffer);
void render_buffer(size_t rows, size_t cols, struct cell ***buffer);

const struct color DEFAULT_FG_COLOR = { .r = 255, .g = 255, .b = 255 };
const struct color DEFAULT_BG_COLOR = { .r = 0, .g = 0, .b = 0 };

const size_t ROWS = 24; 
const size_t COLS = 80; 

int main() {
    setlocale(LC_CTYPE, "");
    fputs("\e[?1049h", stdout);
    struct cell **buffer;
    init_buffer(ROWS, COLS, &buffer);
    render_buffer(ROWS, COLS, &buffer);
    getc(stdin);
    free_buffer(ROWS, &buffer);
    fputs("\e[?1049l", stdout);

    return 0;
}

void init_buffer(size_t rows, size_t cols, struct cell ***buffer) {
    *buffer = (struct cell **)malloc(rows * sizeof(struct cell *));
    for(size_t i = 0; i < rows; i++) {
        (*buffer)[i] = (struct cell *)malloc(cols * sizeof(struct cell));
        for(size_t j = 0; j < cols; j++) {
            (*buffer)[i][j].character = L' ';
            (*buffer)[i][j].fg_color = DEFAULT_FG_COLOR;
            (*buffer)[i][j].bg_color = DEFAULT_BG_COLOR;
        }
    }
}

void free_buffer(size_t rows, struct cell ***buffer) {
    for(size_t i = 0; i < rows; i++) {
        free((*buffer)[i]);
    }
    free(*buffer);
}

bool eq_colors(struct color *first, struct color *second) {
    return first->r == second->r &&
        first->g == second->g &&
        first->b == second->b;
}

void render_buffer(size_t rows, size_t cols, struct cell ***buffer) {
    struct color prev_fg_color = DEFAULT_FG_COLOR;
    struct color prev_bg_color = DEFAULT_BG_COLOR;
    fputs("\e[0m", stdout);
    fputs("\e[2J", stdout);
    fputs("\e[H", stdout);
    printf("\e[38;2;%d;%d;%dm", DEFAULT_FG_COLOR.r, DEFAULT_FG_COLOR.g, DEFAULT_FG_COLOR.b);
    printf("\e[48;2;%d;%d;%dm", DEFAULT_BG_COLOR.r, DEFAULT_BG_COLOR.g, DEFAULT_BG_COLOR.b);
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            struct cell *cur_cell = &((*buffer)[i][j]);
            if(!eq_colors(&(cur_cell->fg_color), &prev_fg_color)) {
                printf("\e[38;2;%d;%d;%dm", cur_cell->fg_color.r, cur_cell->fg_color.g, cur_cell->fg_color.b);
            }
            if(!eq_colors(&(cur_cell->bg_color), &prev_bg_color)) {
                printf("\e[48;2;%d;%d;%dm", cur_cell->bg_color.r, cur_cell->bg_color.g, cur_cell->bg_color.b);
            }
            fputc(cur_cell->character, stdout);
        }
        putc('\n', stdout);
    }
}
