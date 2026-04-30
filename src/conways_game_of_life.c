#include "../include/conways_game_of_life.h"
#include <stdlib.h>
#include <stdint.h>
#include "../include/utils.h"
#include <string.h>
#include <assert.h>

int init_conways(Conways* game_of_life, size_t rows, size_t collumns, float alive_prob) {
    if(!game_of_life) {
        return -1;
    }

    game_of_life->rows = rows;
    game_of_life->collumns = collumns;
    game_of_life->alive_prob = alive_prob;
    game_of_life->total_buff_size = (((rows + 2) * (collumns + 2) + 7) / 8);
    game_of_life->cells = (uint8_t*)calloc(game_of_life->total_buff_size, sizeof(uint8_t));
    game_of_life->next_cells = (uint8_t*)calloc(game_of_life->total_buff_size, sizeof(uint8_t));
    if(!game_of_life->cells) {
        return -1;
    }

    if(!game_of_life->next_cells) {
        free(game_of_life->cells);
        return -1;
    }

    // fill with given prob
    for(size_t i = rows; i-- > 0;) {
        for(size_t j = collumns; j-- > 0;) {
            size_t idx = (i + 1) * (collumns + 2) + (j + 1);
            size_t byte = idx / 8;
            size_t bit = idx % 8;
            game_of_life->cells[byte] |= (((rand() / (double)RAND_MAX) < alive_prob? 1 : 0) << bit);
        }
    }

    return 0;
}

int draw_conways(Conways* game_of_life) {
    return fdraw_conways(game_of_life, stdout);
}

int fdraw_conways(Conways* game_of_life, FILE* restrict out_desc) {
    clear_screen();
    reset_cursor();

    char print_buff[OUTPUT_BUFF_SIZE];
    uint32_t buff_size = 0;

    if(!game_of_life || !game_of_life->cells) {
        return -1;
    }

    for(uint64_t i = 0; i < game_of_life->rows; ++i) {
        for(uint64_t j = 0; j < game_of_life->collumns; ++j) {
            if(buff_size >= OUTPUT_BUFF_SIZE) {
                fwrite(print_buff, 1, buff_size, out_desc);
                buff_size = 0;
            }
            print_buff[buff_size++] = unsafe_get_cell_state(game_of_life, i, j) == 1? ALIVE_CELL_STR : DEAD_CELL_STR;
        }

        if(buff_size >= OUTPUT_BUFF_SIZE) {
            fwrite(print_buff, 1, buff_size, out_desc);
            buff_size = 0;
        }
        print_buff[buff_size++] = '\n';
    }

    fwrite(print_buff, 1, buff_size, out_desc);
    return 0;
}

void unsafe_update_cell(const Conways* game_of_life, const size_t row, const size_t collumn) {
    size_t neighbour_cnt = 0;
    int nx;
    int ny;

    for(int dx = -1; dx <= 1; ++dx) {
        for(int dy = -1; dy <= 1; ++dy) {
            if(dx == 0 && dy == 0) {
                continue;
            }

            ny = (int)row + dy;
            nx = (int)collumn + dx;

            neighbour_cnt += unsafe_get_cell_state(game_of_life, ny, nx);
        }
    }
    size_t idx = (row + 1) * (game_of_life->collumns + 2) + (collumn + 1);
    size_t byte = idx >> 3;
    size_t bit = idx & 7;
    if(unsafe_get_cell_state(game_of_life, row, collumn)) {
        //printf("Hello");
        // dabartinas gyvas, jei turi 2 arba 3 kaimynus -> gyvas
        if(neighbour_cnt == 2 || neighbour_cnt == 3) {
            game_of_life->next_cells[byte] |= (1 << bit);
        }
        else {
            game_of_life->next_cells[byte] &= ~(1 << bit);
        }
    }
    else {
        if(neighbour_cnt == 3) {
            game_of_life->next_cells[byte] |= (1 << bit);
        }
        else {
            game_of_life->next_cells[byte] &= ~(1 << bit);
        }
    }
}

int update_conways(Conways* game_of_life) {
     if(!game_of_life || !game_of_life->cells || !game_of_life->next_cells){
        return -1;
    }

    for(size_t i = game_of_life->rows; i-- > 0;) {
        for(size_t j = game_of_life->collumns; j-- > 0;) {
            unsafe_update_cell(game_of_life, i, j);
        }
    }

    unsafe_swap_buffers(game_of_life);
    return 0;
}

void unsafe_swap_buffers(Conways* game_of_life) {
    uint8_t* temp = game_of_life->cells;
    game_of_life->cells = game_of_life->next_cells;
    game_of_life->next_cells = temp;
}


void delete_conways(Conways* game_of_life) {
    if(!game_of_life) {
        return;
    }

    if(game_of_life->next_cells) {
        free(game_of_life->next_cells);
    }
    if(game_of_life->cells) {
        free(game_of_life->cells);
    }
}

uint8_t get_cell_state(const Conways* game_of_life, const size_t row, const size_t column) {
    if(!game_of_life || !game_of_life->cells) {
        return 0xFF;
    }

    if(row >= game_of_life->rows || column >= game_of_life->collumns) {
        return 0xFF;
    }

    size_t idx = (row + 1) * (game_of_life->collumns + 2) + (column + 1);
    size_t byte = idx >> 3;
    size_t bit = idx & 7;

    return (game_of_life->cells[byte] >> bit) & 1;
}

uint8_t unsafe_get_cell_state(const Conways* game_of_life, const int row, const int collumn) {
    size_t idx = (row + 1) * (game_of_life->collumns + 2) + (collumn + 1);
    size_t byte = idx >> 3;
    size_t bit = idx & 7;

    // printf("%d, %d, %lu, %lu, %lu\n", row, collumn, idx, byte, bit);

    return (game_of_life->cells[byte] >> bit) & 1;
}

int unsafe_update_block(Conways* game_of_life, size_t start_row, size_t start_col, size_t size_row, size_t size_col) {
    if(!game_of_life || !game_of_life->cells || !game_of_life->next_cells) {
        return -1;
    }

    size_t end_col;
    size_t end_row;

    if(start_col + size_col >= game_of_life->collumns) {
        end_col = game_of_life->collumns;
    }
    else {
        end_col = start_col + size_col;
    }

    // top 5 legendary bugs were here
    if(start_row + size_row >= game_of_life->rows) {
        end_row = game_of_life->rows;
    }
    else {
        end_row = start_row + size_row;
    }

    for(size_t i = start_row; i < end_row; ++i) {
        for(size_t j = start_col; j < end_col; ++j) {
            unsafe_update_cell(game_of_life, i, j);
        }
    }

    return 0;
}

uint8_t* get_row_ptr(Conways* game_of_life, size_t row) {
    if(!game_of_life || !game_of_life->cells) {
        return NULL;
    }

    if(row >= game_of_life->rows) {
        return NULL;
    }

    // account for inner padding
    size_t byte = (row + 1) * (game_of_life->collumns + 2) / 8;
    return game_of_life->cells + byte;
}

int update_ghost_top(Conways* game_of_life, uint8_t* ghost_row) {
    if(!game_of_life || !game_of_life->cells || !ghost_row) {
        return -1;
    }

    // beggining of the array points to the ghost top row
    memcpy(game_of_life->cells, ghost_row, (game_of_life->collumns + 2) / 8);
    return 0;
}

int update_ghost_bottom(Conways* game_of_life, uint8_t* ghost_row) {
    if(!game_of_life || !game_of_life->cells || !ghost_row) {
        return -1;
    }

    // beggining of the array points to the ghost top row
    size_t offset = (game_of_life->rows + 1) * ((game_of_life->collumns + 2) / 8);
    memcpy(game_of_life->cells + offset, ghost_row, (game_of_life->collumns + 2) / 8);
    return 0;
}

void draw_stripe(uint8_t* row, size_t cols) {
    if(!row) {
        return;
    }

    // avoid first ghost col
    for(size_t i = 0; i < cols; ++i) {
        size_t bit_idx = i + 1;
        size_t byte = bit_idx / 8;
        size_t bit = bit_idx % 8;
        printf("%c", (row[byte] & 1 << bit? ALIVE_CELL_STR: DEAD_CELL_STR));
    }
    printf("\n");
}


int load_conway_from_file(Conways* game_of_life, const char* filename) {
    if(!game_of_life || !filename) {
        return -1;
    }

    FILE* fptr = fopen(filename, "r");
    if(!fptr) {
        return -1;
    }

    // read the num of rows and cols
    size_t rows = 0;
    size_t cols = 0;
    float alive_prob = 0.2;
    if(fscanf(fptr, "%zu %zu %f", &rows, &cols, &alive_prob) != 3) {
        fclose(fptr);
        return -1;
    };

    assert((cols + 2) % 8 == 0);

    init_conways(game_of_life, rows, cols, alive_prob);
    for(size_t i = 0; i < rows; ++i) {
        for(size_t j = 0; j < cols; ++j) {
            uint8_t state = 0;
            if(fscanf(fptr, "%hhu", &state) != 1) {
                fclose(fptr);
                return -1;
            }

            size_t idx = (i + 1) * (cols + 2) + (j + 1);
            size_t byte = idx >> 3;
            size_t bit = idx & 7;
            game_of_life->cells[byte] |= (state) << bit;
        }
    }

    fclose(fptr);
    return 0;
}

int save_conway_to_file(Conways* game_of_life, const char* filename) {
    if(!game_of_life || !game_of_life->cells || !filename) {
        return -1;
    }

    FILE* fptr = fopen(filename, "w");
    if(!fptr) {
        return -1;
    }

    fprintf(fptr, "%zu %zu %f\n", game_of_life->rows, game_of_life->collumns, game_of_life->alive_prob);
    for(size_t i = 0; i < game_of_life->rows; ++i) {
        for(size_t j = 0; j < game_of_life->collumns; ++j) {
            fprintf(fptr, "%hhu ", get_cell_state(game_of_life, i, j));
        }

        fputc('\n', fptr);
    }

    fclose(fptr);
    return 0;
}

int fdraw_stripe_binary(uint8_t* row, size_t cols, FILE* restrict stream) {
    if(!stream || !row) {
        return -1;
    }

    for(size_t i = 0; i < cols; ++i) {
        size_t idx = i + 1;
        size_t byte = idx >> 3;
        size_t bit = idx & 7;
        fprintf(stream, "%hhu ", (row[byte] & (1 << bit)) ? 1 : 0);
    }
    fputc('\n', stream);
    return 0;
}

int fread_stripe_binary(uint8_t* row_buff, size_t cols, FILE* restrict stream) {
    if(!stream || !row_buff) {
        return -1;
    }
    for(size_t i = 0; i < cols; ++i) {
        size_t idx = i + 1;
        size_t byte = idx >> 3;
        size_t bit = idx & 7;
        uint8_t state = 0;
        fscanf(stream, "%hhu", &state);
        row_buff[byte] |= (state << bit);
    }

    return cols;
}

