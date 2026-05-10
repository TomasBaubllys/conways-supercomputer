#!/bin/bash

#SBATCH -n128
#SBATCH -c4
#SBATCH -o rese128.csv

module load openmpi

mpirun -n 128 ../parallel_game_of_life -r102400 -c79998 -t4 -bs512 -e10
