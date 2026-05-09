#!/bin/bash
#SBATCH -p main
#SBATCH -n 128
#SBATCH -o res128.csv

module load openmpi

sizes=("1024,798" "10240,7998" "1024,79998" "10240,79998" "102400,79998")

for pair in "${sizes[@]}"; do
    IFS=',' read -r R C <<< "$pair"
    echo -n "$R,$C,"
    mpirun -n 128 ../parallel_game_of_life -r$R -c$C -e10
done