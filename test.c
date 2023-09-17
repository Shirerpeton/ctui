#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "tui.h"

const int DELAY = 1.0 / 24.0 * 1000000000;

int main() {
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

    struct tui *tui = init_tui();

    char seq[3];
    unsigned int frame = 0;
    const struct timespec req = { .tv_sec = 0, .tv_nsec = DELAY };
    while(true) {
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
