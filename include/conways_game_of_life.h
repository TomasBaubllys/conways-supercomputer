/* Author: Tomas Baublys
 * File contents: declarations of conways game of life structs and according functions
 * */

#ifndef CONWAYS_GAME_OF_LIFE_H_INCLUDED
#define CONWAYS_GAME_OF_LIFE_H_INCLUDED

#define ALIVE_CELL_STR '#'
#define DEAD_CELL_STR ' '
#define OUTPUT_BUFF_SIZE 512

#include <stdint.h>
#include <stdio.h>

typedef struct Conways {
  size_t rows;
  size_t collumns;
  uint8_t* cells; // stores cells in groups by 8
  uint8_t* next_cells;
  size_t total_buff_size;
} Conways;

// inits conways struct
int init_conways(Conways* game_of_life, size_t rows, size_t collumns, float alive_prob);

// draws the current state on the screen
int draw_conways(Conways* game_of_life);

int fdraw_conways(Conways* game_of_life, FILE* restrict out_desc);

int load_conway_from_file(const char* filename);

int save_conway_to_file(const char* filename);

// plays a single round, returns how many cells are left alive
int update_conways(Conways* game_of_life);

// updates a block does not check for null
int unsafe_update_block(Conways* game_of_life, size_t start_row, size_t start_col, size_t size_row, size_t size_col);

void delete_conways(Conways* game_of_life);

// does not check for NULL
static inline void unsafe_update_cell(const Conways* game_of_life, const size_t row, const size_t collumn);

uint8_t get_cell_state(const Conways* game_of_life, const size_t row, const size_t column);

static inline uint8_t unsafe_get_cell_state(const Conways* game_of_life, const int row, const int column);

// does not check for null
void unsafe_swap_buffers(Conways* game_of_life);

// returns a raw pointer to the row
uint8_t* get_row_ptr(Conways* game_of_life, size_t row);

int update_ghost_top(Conways* game_of_life, uint8_t* ghost_row);

int update_ghost_bottom(Conways* game_of_life, uint8_t* ghost_row);

void draw_stripe(uint8_t* row, size_t cols);

#endif // CONWAYS_GAME_OF_LIFE_H_INCLUDED
