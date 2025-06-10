cmake_minimum_required(VERSION 3.10)

# flock
project(flock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# deps

add_subdirectory(deps/glfw)

file(GLOB_RECURSE SRC_FILES
	src/*.cpp
	deps/glad/src/gl.c
)

add_executable(flock ${SRC_FILES})

target_include_directories(flock PRIVATE 
    src
	deps/glfw/include
	deps/glad/include
	deps/glm
)

target_link_libraries(flock PRIVATE glfw Opengl32)