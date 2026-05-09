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

long long get_elapsed_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) *  1000000000LL + (end.tv_nsec - start.tv_sec);
}
