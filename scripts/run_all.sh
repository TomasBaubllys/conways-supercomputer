#!/bin/bash

FILE="../parallel_game_of_life"
BUILD_DIR="../build"
PROJECT_ROOT=".."

if [ ! -x "$FILE" ]; then
    sbatch --wait build.sh
fi

if [ -x "$FILE" ]; then
    for script in $(ls -v job*.sh 2>/dev/null); do
        sbatch "$script"
    done
else
    echo "Build failed, jobs not submitted."
    exit 1
fi