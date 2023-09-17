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

typedef struct cell **buffer;

void init_buffer(size_t rows, size_t cols, buffer *buf);
void free_buffer(size_t rows, buffer *buf);
void render_buffer(size_t rows, size_t cols, buffer *buf);
void render_buffer_to_str(size_t rows, size_t cols, buffer *buf, size_t str_buf_size, wchar_t **str_buf);

const struct color DEFAULT_FG_COLOR = { .r = 255, .g = 255, .b = 255 };
const struct color DEFAULT_BG_COLOR = { .r = 0, .g = 0, .b = 0 };
const bool NO_DEFAULT_BG_COLOR = true; 
const size_t ROWS = 24; 
const size_t COLS = 80; 

int main() {
    setlocale(LC_CTYPE, "");
    fputws(L"\e[?1049h", stdout);
    fflush(stdout);
    size_t str_buf_size = ROWS * COLS * sizeof(wchar_t) * 20 + 1;
    wchar_t *str_buf = malloc(str_buf_size); 
    buffer buf;
    init_buffer(ROWS, COLS, &buf);
    buf[10][10].character = L'E';
    render_buffer_to_str(ROWS, COLS, &buf, str_buf_size, &str_buf);
    fputws(str_buf, stdout);
    fflush(stdout);
    getc(stdin);
    free_buffer(ROWS, &buf);
    free(str_buf);
    fputws(L"\e[?1049l", stdout);

    return 0;
}

void init_buffer(size_t rows, size_t cols, buffer *buf) {
    *buf = malloc(rows * sizeof(struct cell *));
    for(size_t i = 0; i < rows; i++) {
        (*buf)[i] = malloc(cols * sizeof(struct cell));
        for(size_t j = 0; j < cols; j++) {
            (*buf)[i][j].character = L' ';
            (*buf)[i][j].fg_color = DEFAULT_FG_COLOR;
            (*buf)[i][j].bg_color = DEFAULT_BG_COLOR;
        }
    }
}

void free_buffer(size_t rows, buffer *buf) {
    for(size_t i = 0; i < rows; i++) {
        free((*buf)[i]);
    }
    free(*buf);
}

bool eq_colors(struct color *first, struct color *second) {
    return first->r == second->r &&
        first->g == second->g &&
        first->b == second->b;
}

void render_buffer(size_t rows, size_t cols, buffer *buf) {
    fputs("\e[0m", stdout);
    fputs("\e[2J", stdout);
    fputs("\e[H", stdout);
    printf("\e[38;2;%d;%d;%dm", DEFAULT_FG_COLOR.r, DEFAULT_FG_COLOR.g, DEFAULT_FG_COLOR.b);
    if(!NO_DEFAULT_BG_COLOR) {
        printf("\e[48;2;%d;%d;%dm", DEFAULT_BG_COLOR.r, DEFAULT_BG_COLOR.g, DEFAULT_BG_COLOR.b);
    } 
    struct color prev_fg_color = DEFAULT_FG_COLOR;
    struct color prev_bg_color = DEFAULT_BG_COLOR;
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            struct cell *cur_cell = &((*buf)[i][j]);
            if(!eq_colors(&(cur_cell->fg_color), &prev_fg_color)) {
                printf("\e[38;2;%d;%d;%dm", cur_cell->fg_color.r, cur_cell->fg_color.g, cur_cell->fg_color.b);
            }
            if(!eq_colors(&(cur_cell->bg_color), &prev_bg_color)) {
                if(NO_DEFAULT_BG_COLOR && eq_colors(&(cur_cell->bg_color), (struct color *)&DEFAULT_BG_COLOR)) {
                    fputs("\e[49m", stdout);
                } else {
                    printf("\e[48;2;%d;%d;%dm", cur_cell->bg_color.r, cur_cell->bg_color.g, cur_cell->bg_color.b);
                }
            }
            fputc(cur_cell->character, stdout);
        }
        putc('\n', stdout);
    }
}

void append_to_str(const wchar_t *src, wchar_t **dest, size_t *offset) {
    wcscpy(*dest + *offset, src);
    *offset += wcslen(src);
}

void append_char_to_str(const wchar_t src, wchar_t **dest, size_t *offset) {
    *(*dest + *offset) = src;
    (*offset)++;
}

void append_color_to_str(const wchar_t *format, wchar_t **dest, size_t dest_size, size_t *offset, struct color color) {
    size_t written = swprintf(*dest + *offset,
            dest_size - *offset,
            format,
            color.r,
            color.g,
            color.b);
    if(written < 0) {
    } else {
        *offset += written;
    }
}

void render_buffer_to_str(size_t rows, size_t cols, buffer *buf, size_t str_buf_size, wchar_t **str_buf) {
    size_t offset = 0;
    append_to_str(L"\e[0m", str_buf, &offset);
    append_to_str(L"\e[2J", str_buf, &offset);
    append_to_str(L"\e[H", str_buf, &offset);
    append_color_to_str(L"\e[38;2;%d;%d;%dm", str_buf, str_buf_size, &offset, DEFAULT_FG_COLOR);
    if(!NO_DEFAULT_BG_COLOR) {
        append_color_to_str(L"\e[48;2;%d;%d;%dm", str_buf, str_buf_size, &offset, DEFAULT_BG_COLOR);
    } 
    struct color prev_fg_color = DEFAULT_FG_COLOR;
    struct color prev_bg_color = DEFAULT_BG_COLOR;
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            struct cell *cur_cell = &((*buf)[i][j]);
            if(!eq_colors(&(cur_cell->fg_color), &prev_fg_color)) {
                append_color_to_str(L"\e[38;2;%d;%d;%dm", str_buf, str_buf_size, &offset, cur_cell->fg_color);
            }
            if(!eq_colors(&(cur_cell->bg_color), &prev_bg_color)) {
                if(NO_DEFAULT_BG_COLOR && eq_colors(&(cur_cell->bg_color), (struct color *)&DEFAULT_BG_COLOR)) {
                    append_to_str(L"\e[49m", str_buf, &offset);
                } else {
                    append_color_to_str(L"\e[48;2;%d;%d;%dm", str_buf, str_buf_size, &offset, cur_cell->bg_color);
                }
            }
            append_char_to_str(cur_cell->character, str_buf, &offset);
        }
        append_char_to_str(L'\n', str_buf, &offset);
    }
}
