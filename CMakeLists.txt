cmake_minimum_required(VERSION 3.10)

# flock
project(flock LANGUAGES CXX)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)

    # Caminho para binário final (flock.exe)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG}/$<PLATFORM_ID>)

    # Caminho para arquivos intermediários (obj, etc)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin-int/${OUTPUTCONFIG}/$<PLATFORM_ID>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin-int/${OUTPUTCONFIG}/$<PLATFORM_ID>)
    set(CMAKE_PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}     ${CMAKE_SOURCE_DIR}/bin-int/${OUTPUTCONFIG}/$<PLATFORM_ID>)
endforeach()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT flock)

# deps

add_subdirectory(deps/glfw)

set(MAIN_SOURCE
    src/main.cpp
)
set(RENDER_SOURCE
    src/render/shader.cpp
    src/render/shader.hpp
    src/render/camera.cpp
    src/render/camera.hpp
    src/render/display.cpp
    src/render/display.hpp
)

set(TEXTURE_SOURCE
    src/texture/texture.cpp
    src/texture/texture.hpp
)

set(UI_SOURCE
    src/ui/imgui_config.hpp
)

set(IMGUI_SOURCE
	deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
)

set(SHADERS_SOURCE
	assets/shaders/f.glsl
	assets/shaders/v.glsl
)


set(GLAD_SOURCE
    deps/glad/src/gl.c
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/render PREFIX "src/render" FILES ${RENDER_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/texture PREFIX "src/texture" FILES ${TEXTURE_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ui PREFIX "src/ui" FILES ${UI_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${MAIN_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui PREFIX "deps/imgui" FILES ${IMGUI_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad PREFIX "deps/glad" FILES ${GLAD_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/assets PREFIX "assets/shaders" FILES ${SHADERS_SOURCE})


add_executable(flock 
	${MAIN_SOURCE}
	${RENDER_SOURCE}
	${IMGUI_SOURCE}
	${UI_SOURCE}
 	${TEXTURE_SOURCE}
	${GLAD_SOURCE}
	${SHADERS_SOURCE}
)

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