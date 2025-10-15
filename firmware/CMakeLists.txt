
cmake_minimum_required(VERSION 3.10)
project(cem_simulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

include_directories(include)

add_executable(cem_sim
    src/main.c
    src/cem.c
    src/hal_stub.c
    src/cli.c
)
