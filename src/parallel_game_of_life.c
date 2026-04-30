#include "../include/parallel_game_of_life.h"
#include <omp.h>

int init_parallel_conways(Parallel_Conways* pgame_of_life, Conways* game_of_life, uint32_t block_size) {
    if(!pgame_of_life || !game_of_life) {
        return -1;
    }

    pgame_of_life->game_of_life = game_of_life;
    pgame_of_life->block_size = (block_size / 8) * 8;
    return 0;
}

int update_parallel_conways(Parallel_Conways* pgame_of_life) {
    if(!pgame_of_life || !pgame_of_life->game_of_life || !pgame_of_life->game_of_life->cells || !pgame_of_life->game_of_life->next_cells) {
        return -1;
    }
    
    #pragma omp parallel for collapse(2) 
    for(size_t i = 0; i < pgame_of_life->game_of_life->rows; i += pgame_of_life->block_size) {
        for(size_t j = 0; j < pgame_of_life->game_of_life->collumns; j += pgame_of_life->block_size) {
            //printf("Hello from %d\n", omp_get_thread_num());
            unsafe_update_block(pgame_of_life->game_of_life, i, j, pgame_of_life->block_size, pgame_of_life->block_size);
        }
    }

    unsafe_swap_buffers(pgame_of_life->game_of_life);

    return 0;
}