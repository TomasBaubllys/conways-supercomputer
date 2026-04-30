cmake_minimum_required(VERSION 3.22)
project(parallel_game_of_life)

set(SOURCE_FILES
    src/main.c
    src/utils.c
    src/parallel_game_of_life.c
    src/conways_game_of_life.c)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -g)

target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_C
                                                OpenMP::OpenMP_C)
