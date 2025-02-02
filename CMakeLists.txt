cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED on)

project(prog VERSION 0.0.1)
file(GLOB_RECURSE SRC_FILES ./src/*.c)
add_executable(prog ${SRC_FILES})
target_link_libraries(prog PUBLIC curses pthread m)
