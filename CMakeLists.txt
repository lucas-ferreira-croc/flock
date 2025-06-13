cmake_minimum_required(VERSION 3.10)

# flock
project(flock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# deps

add_subdirectory(deps/glfw)

set(IMGUI_SOURCES
	deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
)

set(SOURCES
	src/main.cpp
	src/render/shader.cpp
	src/render/camera.cpp
	src/texture/texture.cpp
	deps/glad/src/gl.c
)

set(HEADERS
	src/render/shader.hpp
	src/render/camera.hpp
	src/texture/texture.hpp
	src/ui/imgui_config.hpp
)

add_executable(flock ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})

target_include_directories(flock PRIVATE 
    src
	deps
	deps/imgui
	deps/imgui/backends
	deps/glfw/include
	deps/glad/include
	deps/glm
)

target_link_libraries(flock PRIVATE glfw Opengl32)