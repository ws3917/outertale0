cmake_minimum_required(VERSION 3.21)
project(psot_sans_battle)
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_mixer REQUIRED CONFIG)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(main SHARED ${SOURCES})
target_link_libraries(main PRIVATE 
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer-shared
)
target_link_libraries(main PRIVATE android log)