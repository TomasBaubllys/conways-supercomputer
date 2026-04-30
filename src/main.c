#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
#include <string.h>
#include <assert.h>

#include "../include/utils.h"
#include "../include/parallel_game_of_life.h"

int main(int argc, char* argv[]) {    
    // rank of the current node
    int rank;
    // number of nodes
    int num_nodes;
    char verbose = 0;
    size_t rows = 0;
    size_t cols = 0;
    float alive_prob = 0.2;
    size_t num_epochs = 10;

    /*
    OpenMP variables, used for multthreading per node
    */
    // threads per node
    uint32_t t_per_node = 1;
    uint32_t bs_size = 64;

    // handle command line args
    for(int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
        
        if(strncmp(argv[i], "-r", 2) == 0) {
            sscanf(argv[i] + 2, "%lu", &rows);
        }

        if(strncmp(argv[i], "-c", 2) == 0) {
            sscanf(argv[i] + 2, "%lu", &cols);
        }

        if(strncmp(argv[i], "-t", 2) == 0) {
            sscanf(argv[i] + 2, "%u", &t_per_node);
        }

        if(strncmp(argv[i], "-bs", 3) == 0) {
            sscanf(argv[i] + 3, "%u", &bs_size);
        }

        if(strncmp(argv[i], "-ap", 3) == 0) {
            sscanf(argv[i] + 3, "%f", &alive_prob);
        }

        if(strncmp(argv[i], "-e", 2) == 0) {
            sscanf(argv[i] + 2, "%lu", &num_epochs);
        }
    }

    // blocksize must be alligned to 8 bytes for bit packing to work
    assert((bs_size % 8) == 0);
    // columns + 2 must be alligned to 8 bytes for bit packing to work (the grid is padded with 2 columns)
    assert(((cols + 2) % 8) == 0);

    // printf("%lu, %lu, %u", rows, cols, t_per_node);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

    srand(rank);
    size_t rows_per_rank = rows / num_nodes;
    size_t start_row = rank * rows_per_rank;
    size_t bytes_per_row = (cols + 2) / 8;

    // if threads per process flag was passed
    if(t_per_node) {
        omp_set_num_threads(t_per_node);
    }
    
    Conways game_of_life;
    init_conways(&game_of_life, rows_per_rank, cols, alive_prob);

    Parallel_Conways pgame_of_life;
    init_parallel_conways(&pgame_of_life, &game_of_life, bs_size);

    uint8_t* my_top_row = NULL;
    uint8_t* top_ghost = calloc(bytes_per_row, sizeof(uint8_t));
    uint8_t* my_bottom_row = NULL;
    uint8_t* bottom_ghost = calloc(bytes_per_row, sizeof(uint8_t));
    uint8_t* zeros = calloc(bytes_per_row, sizeof(uint8_t));

    MPI_Status status;

    uint8_t* recv_buff = NULL;
    if(verbose && rank == 0) {
        recv_buff = malloc(game_of_life.total_buff_size * sizeof(uint8_t));
        if(!recv_buff) {
            return -1;
        }
    }

    for(int i = 0; i < num_epochs; ++i) {
        my_top_row = get_row_ptr(&game_of_life, 0);
        if(!my_top_row) {
            break;
        }
        my_bottom_row = get_row_ptr(&game_of_life, game_of_life.rows - 1);
        if(!my_bottom_row) {
            break;
        }

        if(rank != 0) {
            // send my top row to the upper node
            // receive my ghost top from the same node
            MPI_Sendrecv(my_top_row, bytes_per_row, MPI_UINT8_T, rank - 1, 1,
            top_ghost, bytes_per_row, MPI_UINT8_T, rank - 1, 2, 
            MPI_COMM_WORLD, &status);
            update_ghost_top(&game_of_life, top_ghost);
        }
        /*
        else {
            update_ghost_top(&game_of_life, zeros);
        }
        */

        if(rank != num_nodes - 1) {
            // send my bottom row to the next node 
            // receive my ghost bottom (next nodes top)
            MPI_Sendrecv(my_bottom_row, bytes_per_row, MPI_UINT8_T, rank + 1, 2,
            bottom_ghost, bytes_per_row, MPI_UINT8_T, rank + 1, 1, 
            MPI_COMM_WORLD, &status);
            update_ghost_bottom(&game_of_life, bottom_ghost);
        }
        /*
        else {
            // update_ghost_bottom(&game_of_life, zeros);
        }
        */
        
        // only the main proccess draws
        if(verbose) {
            if(rank == 0) {
                clear_screen();
                reset_cursor();
                // skip over the ghost row
                for(size_t j = bytes_per_row; j < game_of_life.total_buff_size - bytes_per_row; j += bytes_per_row) {
                    draw_stripe(game_of_life.cells + j, cols);
                }
                for(int r = 1; r < num_nodes; ++r) {
                    MPI_Recv(recv_buff, game_of_life.total_buff_size, MPI_UINT8_T, r, 0, MPI_COMM_WORLD, &status);
                    int recv_bytes = 0;
                    MPI_Get_count(&status, MPI_UINT8_T, &recv_bytes);
                    // skip over the ghost row
                    // and not print the bottom ghost row
                    for(size_t j = bytes_per_row; j < recv_bytes - bytes_per_row; j += bytes_per_row) {
                        draw_stripe(recv_buff + j, cols);
                    }
                }
                wait_ms(50000);
            }
            else {
                MPI_Send(game_of_life.cells, game_of_life.total_buff_size, MPI_UINT8_T, 0, 0, MPI_COMM_WORLD);
            }
        }

        //update_conways(&game_of_life);
        if(t_per_node > 1) {
            update_parallel_conways(&pgame_of_life);
        }
        else {
            update_conways(&game_of_life);
        }
    }

    delete_conways(&game_of_life);
    free(top_ghost);
    free(bottom_ghost);
    MPI_Finalize();
    return 0;
}
