#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "tui.h"
#include "tui-elements.h"

const int DELAY = 1.0 / 24.0 * 1000000000;
const unsigned int TOTAL_STEPS = 24;
const size_t ROWS = 30;
const size_t COLS = 100;

void set_rand_color(struct color *color) {
    color->r = rand() % 255;
    color->g = rand() % 255;
    color->b = rand() % 255;
}

unsigned char interpolate(unsigned char a, unsigned char b, unsigned int step) {
    double change = ((a - b) / (double)(TOTAL_STEPS - step));
    return b + change;
}

void interpolate_color(struct color *new_color, struct color *curr_color, unsigned int step) {
    curr_color->r = interpolate(new_color->r, curr_color->r, step);
    curr_color->g = interpolate(new_color->g, curr_color->g, step);
    curr_color->b = interpolate(new_color->b, curr_color->b, step);
}

int main() {
    srand(time(NULL));
    setlocale(LC_CTYPE, "");

    fputws(L"\e[?1049h", stdout); //enable alternate buffer
    fputws(L"\e[?25l", stdout); //set cursor invisible
    fflush(stdout);

    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    int fcntl_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, fcntl_flags | O_NONBLOCK);

    struct tui *tui = init_tui(ROWS, COLS);

    char seq[3];
    unsigned int frame = 0;
    const struct timespec req = { .tv_sec = 0, .tv_nsec = DELAY };
    struct color curr_fg_color = { .r = 255, .g = 255, .b = 0 };
    struct print_options print_opts = { .x = 10, .y = 10, .fg_color = &curr_fg_color, .bg_color = NULL };
    struct color new_fg_color;
    set_rand_color(&new_fg_color);
    unsigned int step;
    size_t loading_bar_length = 50;
    wchar_t *loading_bar = malloc(loading_bar_length * sizeof(wchar_t));
    while(true) {
        step = frame % TOTAL_STEPS; 
        if(step == 0) {
            set_rand_color(&new_fg_color);
        } else {
            interpolate_color(&new_fg_color, &curr_fg_color, step);
        }
        clear(tui);
        print_tui(tui, print_opts, L"something");
        get_loading_bar(&loading_bar, loading_bar_length, frame % 100);
        print_opts.y = 15;
        print_tui(tui, print_opts, loading_bar);
        print_opts.x = print_opts.x + loading_bar_length;
        wchar_t percent[5];
        swprintf(percent, 5, L"%d%%", frame % 100);
        print_tui(tui, print_opts, percent);
        print_opts.x = 10;
        print_opts.y = 20;
        wchar_t temp[50];
        swprintf(temp, 50, L"r: %d", curr_fg_color.r);
        print_tui(tui, print_opts, temp);
        print_opts.y = 21;
        swprintf(temp, 50, L"g: %d", curr_fg_color.g);
        print_tui(tui, print_opts, temp);
        print_opts.y = 22;
        swprintf(temp, 50, L"b: %d", curr_fg_color.b);
        print_tui(tui, print_opts, temp);
        print_opts.y = 10;
        refresh(tui);
        if(read(STDIN_FILENO, &seq[0], 1) > 0) {
            if(seq[0] == '\e') {
                if(read(STDIN_FILENO, &seq[1], 1) > 0 &&
                   read(STDIN_FILENO, &seq[2], 1) > 0) {
                    if(seq[1] == '[') {
                        switch(seq[2]) {
                            case 'A': //up arrow
                                break;
                            case 'B': //down arrow
                                break;
                            case 'C': //right arrow
                                break;
                            case 'D': //left arrow
                                break;
                        }
                    }
                } else {
                    break;
                }
            } else if(seq[0] == 'q') {
                break;
            }
        } else {
            wprintf(L"frame: %d\n", frame++);
            nanosleep(&req, NULL);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    fputws(L"\e[?1049l", stdout); //disable alternate buffer
    fputws(L"\e[?25h", stdout); //set cursor invisible

    free_tui(tui);

    return 0;
}
