#ifndef TUI_ELEMENTS_H
#define TUI_ELEMENTS_H
#include <stdlib.h>

struct menu {
    wchar_t **options;
    size_t options_size;
    unsigned char selected;
    struct color *fg_color;
    struct color *bg_color;
    struct color *selected_fg_color;
    struct color *selected_bg_color;
    unsigned int x;
    unsigned int y;
};

int get_loading_bar(wchar_t **loading_bar, size_t length, unsigned char pct);
void print_borders(struct tui *tui, struct color *fg_color, struct color *bg_color);
void print_menu(struct tui *tui, struct menu *menu);

#endif
