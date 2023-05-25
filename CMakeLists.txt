cmake_minimum_required(VERSION 3.10)

project(minigame VERSION 1.0)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.c
        src/mAlloc.c
        src/mg.c
        src/mProfiler.c
        src/SDL_backend.c)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)