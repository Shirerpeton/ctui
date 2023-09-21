#include <stdlib.h>
#include <stdio.h>
#include "tui.h"

int get_loading_bar(wchar_t **loading_bar, size_t length, unsigned char percentage) {
    if(length <= 3 || percentage > 100) {
        return -1;
    }
    (*loading_bar)[0] = L'[';
    size_t fill_length = (percentage / 100.0 * (length - 3));
    for(size_t i = 1; i < length - 2; i++) {
        if(i <= fill_length) {
            (*loading_bar)[i] = L'=';
        } else {
            (*loading_bar)[i] = L'-';
        }
    }
    (*loading_bar)[length - 2] = L']';
    (*loading_bar)[length - 1] = L'\0';
    return 0;
}

void print_borders(struct tui *tui, struct color *fg_color, struct color *bg_color) {
    struct print_options print_opts = { .x = 0, .y = 0, .fg_color = fg_color, .bg_color = bg_color };
    wchar_t border_line[tui->cols];
    wchar_t line[tui->cols];
    border_line[0] = L'┏';
    for(size_t i = 1; i < tui->cols - 2; i++) {
        border_line[i] = L'━';
    }
    border_line[tui->cols - 2] = L'┓';
    border_line[tui->cols - 1] = L'\0';
    print_tui(tui, print_opts, border_line);
    line[0] = L'┃';
    for(size_t i = 1; i < tui->cols - 2; i++) {
        line[i] = L' ';
    }
    line[tui->cols - 2] = L'┃';
    line[tui->cols - 1] = L'\0';
    for(size_t i = 1; i < tui->rows - 1; i++) {
        print_opts.y = i;
        print_tui(tui, print_opts, line);
    }
    print_opts.y = tui->rows - 1;
    border_line[0] = L'┗';
    border_line[tui->cols - 2] = L'┛';
    print_tui(tui, print_opts, border_line);
}
