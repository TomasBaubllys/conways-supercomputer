#!/bin/bash

#SBATCH -n64
#SBATCH -c8
#SBATCH -o rese64.csv

module load openmpi

mpirun -n 64 ../parallel_game_of_life -r102400 -c79998 -t8 -bs512 -e10
