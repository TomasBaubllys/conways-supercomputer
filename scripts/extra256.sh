#!/bin/bash

#SBATCH -n256
#SBATCH -c2
#SBATCH -o rese256.csv

module load openmpi

mpirun -n 256 ../parallel_game_of_life -r102400 -c79998 -t2 -bs512 -e10
