#define _XOPEN_SOURCE
#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>
#include <unistd.h>
#include "tui.h"

void init_buffer(buffer *buf, size_t rows, size_t cols);
void free_buffer(buffer *buf, size_t rows, size_t cols);
void init_str_buffer(size_t capacity, struct str_buffer *str_buf);
void free_str_buffer(struct str_buffer *str_buf);
void render_buffer_to_str(buffer *buf, struct str_buffer *str_buf, size_t rows, size_t cols);

const struct color DEFAULT_FG_COLOR = { .r = 255, .g = 255, .b = 255 };
const struct color DEFAULT_BG_COLOR = { .r = 10, .g = 10, .b = 10 };
const unsigned int MAX_CHARS_PER_CELL = 5;
const bool NO_DEFAULT_BG_COLOR = true; 

struct tui *init_tui(size_t rows, size_t cols) {
    struct tui *tui = malloc(sizeof(struct tui));
    tui->rows = rows;
    tui->cols = cols;
    tui->debug = malloc(cols * sizeof(wchar_t));
    tui->debug[0] = L'\0';
    init_buffer(&tui->buf, rows, cols);
    size_t str_buf_size = rows * cols * sizeof(wchar_t) * 20 + 1;
    init_str_buffer(str_buf_size, &tui->str_buf);
    return tui;
}

void free_tui(struct tui *tui) {
    free_buffer(&tui->buf, tui->rows, tui->cols);
    free_str_buffer(&tui->str_buf);
    free(tui->debug);
    free(tui);
}

void refresh(struct tui *tui) {
    render_buffer_to_str(&tui->buf, &tui->str_buf, tui->rows, tui->cols);
    fputws(tui->str_buf.data, stdout);
    if(tui->debug[0] != L'\0') {
        fputws(tui->debug, stdout);
    }
    fflush(stdout);
}

void flush_cell_buf(
        struct cell *cur_cell,
        struct print_options *print_opt,
        wchar_t *cell_buf,
        unsigned int cell_buf_len,
        unsigned int cell_buf_width) {
    wcsncpy(cur_cell->content, cell_buf, cell_buf_len);
    cur_cell->content[cell_buf_len] = L'\0';
    cur_cell->width = cell_buf_width;
    if(print_opt->fg_color != NULL) {
        cur_cell->fg_color.r = print_opt->fg_color->r;
        cur_cell->fg_color.g = print_opt->fg_color->g;
        cur_cell->fg_color.b = print_opt->fg_color->b;
    }
    if(print_opt->bg_color != NULL) {
        cur_cell->bg_color.r = print_opt->bg_color->r;
        cur_cell->bg_color.g = print_opt->bg_color->g;
        cur_cell->bg_color.b = print_opt->bg_color->b;
    }
}

int print_tui_len(struct tui *tui, struct print_options print_opt, wchar_t *str, unsigned int len) {
    unsigned int max_width = (tui->cols - print_opt.x) * MAX_CHARS_PER_CELL;
    if(print_opt.x > tui->cols ||
        print_opt.y > tui->rows) {
        return -1;
    }
    wchar_t cell_buf[MAX_CHARS_PER_CELL - 2];
    unsigned int cell_buf_len = 0;
    unsigned int cell_buf_width = 0;
    struct cell *cur_cell = &tui->buf[print_opt.y][print_opt.x];
    unsigned int total_width = 0;
    for(int i = 0; i < len; i++) {
        wchar_t ch = str[i];
        if(ch == L'\n' || ch == L'\0') {
            continue;
        }
        unsigned int char_width = wcwidth(ch);
        if(char_width == 0 || cell_buf_width == 0) {
            if(cell_buf_len < MAX_CHARS_PER_CELL - 2) {
                cell_buf[cell_buf_len] = ch; 
                cell_buf_len++;
                cell_buf_width += char_width;
            } 
        } else {
            flush_cell_buf(cur_cell, &print_opt, cell_buf, cell_buf_len, cell_buf_width);
            total_width += cell_buf_width;
            if(total_width > max_width) {
                return -1;
            }
            cur_cell += cell_buf_width;
            cell_buf[0] = ch;
            cell_buf_len = 1;
            cell_buf_width = char_width;
        }
    }
    if(cell_buf_len > 0) {
        flush_cell_buf(cur_cell, &print_opt, cell_buf, cell_buf_len, cell_buf_width);
    }
    return 0;
}

int print_tui(struct tui *tui, struct print_options print_opt, wchar_t *str) {
    return print_tui_len(tui, print_opt, str, wcslen(str));
}

void debug_tui(struct tui *tui, wchar_t *str) {
    wcsncpy(tui->debug, str, tui->cols);
}

void clear(struct tui *tui) {
    for(int i = 0; i < tui->rows; i++) {
        for(int j = 0; j < tui->cols; j++) {
            struct cell *curr_cell = &tui->buf[i][j];
            curr_cell->content[0] = L' ';
            curr_cell->content[1] = L'\0';
            curr_cell->width = 1;
            curr_cell->fg_color = DEFAULT_FG_COLOR;
            curr_cell->bg_color = DEFAULT_BG_COLOR;
        }
    }
}

void init_buffer(buffer *buf, size_t rows, size_t cols) {
    *buf = malloc(rows * sizeof(struct cell *));
    for(size_t i = 0; i < rows; i++) {
        (*buf)[i] = malloc(cols * sizeof(struct cell));
        for(size_t j = 0; j < cols; j++) {
            (*buf)[i][j].content = malloc(MAX_CHARS_PER_CELL * sizeof(wchar_t));
            (*buf)[i][j].content[0] = L' ';
            (*buf)[i][j].content[1] = L'\0';
            (*buf)[i][j].fg_color = DEFAULT_FG_COLOR;
            (*buf)[i][j].bg_color = DEFAULT_BG_COLOR;
        }
    }
}

void free_buffer(buffer *buf, size_t rows, size_t cols) {
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            free((*buf)[i][j].content);
        }
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

void render_buffer_to_str(buffer *buf, struct str_buffer *str_buf, size_t rows, size_t cols) {
    str_buf->length = 0;
    append_to_str(L"\e[0m\e[2J\e[H", str_buf); //reset mode, erase screen, return cursor to home position
    append_color_to_str(L"\e[38;2;%d;%d;%dm", str_buf, DEFAULT_FG_COLOR);
    if(!NO_DEFAULT_BG_COLOR) {
        append_color_to_str(L"\e[48;2;%d;%d;%dm", str_buf, DEFAULT_BG_COLOR);
    } 
    struct color prev_fg_color = DEFAULT_FG_COLOR;
    struct color prev_bg_color = DEFAULT_BG_COLOR;
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols;) {
            struct cell *cur_cell = &(*buf)[i][j];
            if(!eq_colors(&cur_cell->fg_color, &prev_fg_color)) {
                append_color_to_str(L"\e[38;2;%d;%d;%dm", str_buf, cur_cell->fg_color);
            }
            if(!eq_colors(&cur_cell->bg_color, &prev_bg_color)) {
                if(NO_DEFAULT_BG_COLOR && eq_colors(&cur_cell->bg_color, (struct color *)&DEFAULT_BG_COLOR)) {
                    append_to_str(L"\e[49m", str_buf);
                } else {
                    append_color_to_str(L"\e[48;2;%d;%d;%dm", str_buf, cur_cell->bg_color);
                }
            }
            append_to_str(cur_cell->content, str_buf);
            j += cur_cell->width;
            prev_fg_color = cur_cell->fg_color;
            prev_bg_color = cur_cell->bg_color;
        }
        append_char_to_str(L'\n', str_buf);
    }
    append_to_str(L"\e[0m", str_buf); //reset mode
}
