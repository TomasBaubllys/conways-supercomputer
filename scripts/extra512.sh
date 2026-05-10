#!/bin/bash

#SBATCH -n512
#SBATCH -c1
#SBATCH -o rese512.csv

module load openmpi

mpirun -n 512 ../parallel_game_of_life -r102400 -c79998 -e10
