#ifndef TUI_ELEMENTS_H
#define TUI_ELEMENTS_H
#include <stdlib.h>

int get_loading_bar(wchar_t **loading_bar, size_t length, unsigned char pct);
void print_borders(struct tui *tui, struct color *fg_color, struct color *bg_color);

#endif
