#include <stdlib.h>

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
