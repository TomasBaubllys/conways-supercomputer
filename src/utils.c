#include "../include/utils.h"
#include <stdio.h>
#include <time.h>

void clear_screen() {
    printf("\033[2J"); // clear screen
}

void reset_cursor() {
    printf("\033[H"); // move cursor to top-left
}

void wait_ms(size_t milliseconds) {
    clock_t start = clock();
    while(clock() < start + milliseconds) {};
}