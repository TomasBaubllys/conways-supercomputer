# Conways-supercomputer
This is an implementation of Conways Game of Life for the Vilnius University HPC supercomputer. It is designed to run with MPI and OpenMP models.

## Compiling
The program can be compiled using cmake like this:
```
cmake -B build
cd build && make
```

## Running the program
The program can be in variety of modes for ex.:
```
mpirun -n 4 parallel_game_of_life -c798 -r1000 -v -sf_test.txt
./parallel_game_of_life -c30 -r100 -lf_test.txt -t4 -bs4
```

### Program flags
-v      Prints each epoch with a delay to stdout  

-r      Number of rows for the grid  

-c      Number of columns for the grid must be (c+2)mod8 === 0  

-t      Threads per node (1 or not provided equals single thread)  

-bs     Used with -t flag to provide block size per thread  

-ap     When initializing a random grid provides the probability for a cell to be alive  

-e      Number of epochs to run for  

-sf_    File to save to the final result  

-lf_    File to load from  

-h      Displays this message  

### Speedup results in VU MIF HPC

NWorkers,Speedup
1,1.0
2,1.8998802112348827
4,3.601765103761692
8,6.69722343673285
16,13.011749400804627
32,22.97679088115266
64,47.10695728409472
128,90.12147689059535
256,180.27611656746683
512,365.59488115132024


