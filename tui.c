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

struct str_buffer {
    wchar_t *data;
    size_t capacity;
    size_t length;
};

void init_buffer(size_t rows, size_t cols, buffer *buf);
void free_buffer(size_t rows, buffer *buf);
void init_str_buffer(size_t capacity, struct str_buffer *str_buffer);
void free_str_buffer(struct str_buffer *str_buffer);
void render_buffer_to_str(size_t rows, size_t cols, buffer *buf, struct str_buffer *str_buffer);

const struct color DEFAULT_FG_COLOR = { .r = 255, .g = 255, .b = 255 };
const struct color DEFAULT_BG_COLOR = { .r = 0, .g = 0, .b = 0 };
const bool NO_DEFAULT_BG_COLOR = false; 
const size_t ROWS = 24; 
const size_t COLS = 80; 

int main() {
    setlocale(LC_CTYPE, "");
    fputws(L"\e[?1049h", stdout);
    fflush(stdout);
    size_t str_buf_size = ROWS * COLS * sizeof(wchar_t) * 20 + 1;
    struct str_buffer str_buf;
    init_str_buffer(str_buf_size, &str_buf);
    buffer buf;
    init_buffer(ROWS, COLS, &buf);
    buf[10][10].character = L'E';
    render_buffer_to_str(ROWS, COLS, &buf, &str_buf);
    fputws(str_buf.data, stdout);
    fflush(stdout);
    getc(stdin);
    free_buffer(ROWS, &buf);
    free_str_buffer(&str_buf);
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

void init_str_buffer(size_t capacity, struct str_buffer *str_buf) {
    str_buf->capacity = capacity;
    str_buf->length = 0;
    str_buf->data = malloc(capacity);
}

void free_str_buffer(struct str_buffer *str_buf) {
    free(str_buf->data);
}

bool eq_colors(struct color *first, struct color *second) {
    return first->r == second->r &&
        first->g == second->g &&
        first->b == second->b;
}

void append_to_str(const wchar_t *src, struct str_buffer *str_buf) {
    wcscpy(str_buf->data + str_buf->length, src);
    str_buf->length += wcslen(src);
}

void append_char_to_str(const wchar_t src, struct str_buffer *str_buf) {
    str_buf->data[str_buf->length] = src;
    str_buf->length++;
}

void append_color_to_str(const wchar_t *format, struct str_buffer *str_buf, struct color color) {
    size_t written = swprintf(str_buf->data + str_buf->length,
            str_buf->capacity - str_buf->length,
            format,
            color.r,
            color.g,
            color.b);
    if(written < 0) {
    } else {
        str_buf->length += written;
    }
}

void render_buffer_to_str(size_t rows, size_t cols, buffer *buf, struct str_buffer *str_buf) {
    str_buf->length = 0;
    append_to_str(L"\e[0m\e[2J\e[H", str_buf);
    append_color_to_str(L"\e[38;2;%d;%d;%dm", str_buf, DEFAULT_FG_COLOR);
    if(!NO_DEFAULT_BG_COLOR) {
        append_color_to_str(L"\e[48;2;%d;%d;%dm", str_buf, DEFAULT_BG_COLOR);
    } 
    struct color prev_fg_color = DEFAULT_FG_COLOR;
    struct color prev_bg_color = DEFAULT_BG_COLOR;
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            struct cell *cur_cell = &((*buf)[i][j]);
            if(!eq_colors(&(cur_cell->fg_color), &prev_fg_color)) {
                append_color_to_str(L"\e[38;2;%d;%d;%dm", str_buf, cur_cell->fg_color);
            }
            if(!eq_colors(&(cur_cell->bg_color), &prev_bg_color)) {
                if(NO_DEFAULT_BG_COLOR && eq_colors(&(cur_cell->bg_color), (struct color *)&DEFAULT_BG_COLOR)) {
                    append_to_str(L"\e[49m", str_buf);
                } else {
                    append_color_to_str(L"\e[48;2;%d;%d;%dm", str_buf, cur_cell->bg_color);
                }
            }
            append_char_to_str(cur_cell->character, str_buf);
        }
        append_char_to_str(L'\n', str_buf);
    }
}
