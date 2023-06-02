cmake_minimum_required(VERSION 3.14)

project(minigame VERSION 1.0)
set(CMAKE_C_STANDARD 99)

#           DOWNLOAD ALL THE SUBMODULES
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

add_executable(${PROJECT_NAME}
        src/main.c
        src/mAlloc.c
        src/mg.c
        src/mProfiler.c
        src/SDL_backend.c)

target_include_directories(${PROJECT_NAME} 
        PUBLIC include
        PUBLIC ext/SDL/include
        PUBLIC ext/SDL_mixer/include
)

target_link_directories(${PROJECT_NAME}
        PUBLIC ext/SDL/src
        PUBLIC ext/SDL_mixer/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer)

add_subdirectory(ext/SDL)
add_subdirectory(ext/SDL_mixer)

add_custom_command(
        TARGET minigame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/
        ${CMAKE_CURRENT_BINARY_DIR}/assets/)