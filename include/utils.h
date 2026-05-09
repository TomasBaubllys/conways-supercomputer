#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdlib.h>
#include <time.h>

void clear_screen();

void reset_cursor();

void wait_ms(size_t milliseconds);

long long get_elapsed_ns(struct timespec start, struct timespec end);

#endif // UTILS_H_INCLUDED
