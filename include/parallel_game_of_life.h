#ifndef PARALLEL_GAME_OF_LIFE_H_INCLUDED
#define PARALLEL_GAME_OF_LIFE_H_INCLUDED

#include "conways_game_of_life.h"

typedef struct Parallel_Conways {
    Conways* game_of_life;
    uint32_t block_size;
} Parallel_Conways;

// inits parallel game of life, conways must be already inited
int init_parallel_conways(Parallel_Conways* pgame_of_life, Conways* game_of_life, uint32_t block_size);

int update_parallel_conways(Parallel_Conways* pgame_of_life);

#endif // PARALLEL_GAME_OF_LIFE_H_INCLUDED