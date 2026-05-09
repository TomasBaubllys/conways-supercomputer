#!/bin/bash
#SBATCH -p main           
#SBATCH -c 4              
#SBATCH -n 1              
#SBATCH --job-name=build 
#SBATCH -o build_log.txt 

module load openmpi cmake

cd ..

rm -rf build
mkdir -p build

echo "Config started"
cmake -S . -B build

echo "Compile started"
cmake --build build -j 4

if [ $? -eq 0 ]; then
    mv ./build/parallel_game_of_life .
    echo "Success!"
else
    echo "Error!"
    exit 1
fi

cd scripts