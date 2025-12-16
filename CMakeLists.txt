cmake_minimum_required(VERSION 3.10)

# flock
project(flock LANGUAGES CXX)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)

    # Caminho para binario final (flock.exe)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG}/$<PLATFORM_ID>)

    # Caminho para arquivos intermediarios (obj, etc)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin-int/${OUTPUTCONFIG}/$<PLATFORM_ID>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/bin-int/${OUTPUTCONFIG}/$<PLATFORM_ID>)
    set(CMAKE_PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}     ${CMAKE_SOURCE_DIR}/bin-int/${OUTPUTCONFIG}/$<PLATFORM_ID>)
endforeach()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT flock)

# deps

add_subdirectory(deps/glfw)

set(INTEGRATION_SOURCE
    src/integration/SO/file_dialog.cpp
    src/integration/SO/file_dialog.hpp
)

set(MAIN_SOURCE
    src/main.cpp
)

set(ECS_SOURCE
    src/ecs/ecs.cpp
    src/ecs/ecs.hpp
)

set(COMPONENTS_SOURCE
    src/ecs/components/transform_component.hpp
    src/ecs/components/rigid_body_component.hpp
    src/ecs/components/mesh_component.hpp
    src/ecs/components/debug_mesh_component.hpp
    src/ecs/components/shader_component.hpp
    src/ecs/components/material_component.hpp
    src/ecs/components/physics_shape_component.hpp
    src/ecs/components/cubemap_component.hpp
    src/ecs/components/id_component.hpp
    src/ecs/components/cloth_component.hpp
    src/ecs/components/fabrik_component.hpp
)

set(SYSTEMS_SOURCE
    src/ecs/systems/movement_system.hpp
    src/ecs/systems/render_system.hpp
    src/ecs/systems/physics_system.hpp
    src/ecs/systems/gui_system.hpp
    src/ecs/systems/picking_system.hpp
    src/ecs/systems/xpbd_system.hpp
    src/ecs/systems/ik_system.hpp
)

set(RENDER_SOURCE
    src/render/shader.cpp
    src/render/shader.hpp
    src/render/camera.cpp
    src/render/camera.hpp
    src/render/display.cpp
    src/render/display.hpp
    src/render/mesh.cpp
    src/render/mesh.hpp
    src/render/model.cpp
    src/render/model.hpp
    src/render/light/base_light.hpp
    src/render/light/base_light.cpp
    src/render/light/directional_light.hpp
    src/render/light/point_light.hpp
    src/render/light/spot_light.hpp
)

set(TEXTURE_SOURCE
    src/texture/texture.cpp
    src/texture/texture.hpp
)

set(GAME_SOURCE
    src/game/game.cpp
    src/game/game.hpp
)

set(UI_SOURCE
    src/ui/imgui_config.hpp
)

set(LOGGER_SOURCE
    src/log/logger.cpp   
    src/log/logger.hpp   
)

set(IMGUI_SOURCE
	deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
)

set(IMGUIZMO_SOURCE
    deps/imGuizmo/ImGuizmo.cpp
    deps/imGuizmo/GraphEditor.cpp
    deps/imGuizmo/ImCurveEdit.cpp
    deps/imGuizmo/ImGradient.cpp
    deps/imGuizmo/ImSequencer.cpp
)

set(SHADERS_SOURCE
	assets/shaders/f.glsl
    assets/shaders/v.glsl
)

set(GLAD_SOURCE
    deps/glad/src/gl.c
)

set(PHYSICS_SOURCE
    src/physics/body.cpp
    src/physics/body.hpp
    src/physics/shape.cpp
    src/physics/shape.hpp
    src/physics/world.cpp
    src/physics/world.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/render PREFIX "src/render" FILES ${RENDER_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/texture PREFIX "src/texture" FILES ${TEXTURE_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ui PREFIX "src/ui" FILES ${UI_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs PREFIX "src/ecs" FILES ${ECS_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/components PREFIX "src/ecs/components" FILES ${COMPONENTS_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/systems PREFIX "src/ecs/systems" FILES ${SYSTEMS_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ui PREFIX "src/ui" FILES ${UI_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ui PREFIX "src/ui" FILES ${UI_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/log PREFIX "src/log" FILES ${LOGGER_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/physics PREFIX "src/physics" FILES ${PHYSICS_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/game PREFIX "src/game" FILES ${GAME_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${MAIN_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui PREFIX "deps/imgui" FILES ${IMGUI_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/deps/imGuizmo PREFIX "deps/imGuizmo" FILES ${IMGUIZMO_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad PREFIX "deps/glad" FILES ${GLAD_SOURCE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/assets PREFIX "assets/shaders" FILES ${SHADERS_SOURCE})


add_executable(flock 
	${MAIN_SOURCE}
	${RENDER_SOURCE}
	${IMGUI_SOURCE}
	${IMGUIZMO_SOURCE}
	${UI_SOURCE}
 	${TEXTURE_SOURCE}
	${GLAD_SOURCE}
	${SHADERS_SOURCE}
    ${LOGGER_SOURCE}
    ${GAME_SOURCE}
    ${ECS_SOURCE}
    ${PHYSICS_SOURCE}
    ${INTEGRATION_SOURCE}
)

add_custom_command(TARGET flock POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/deps/assimp/bin/assimp-vc143-mt.dll"
        "$<TARGET_FILE_DIR:flock>"
)

target_include_directories(flock PRIVATE 
    src
	deps
	deps/imgui
	deps/imgui/backends
	deps/glfw/include
	deps/glad/include
	deps/glm
    deps/assimp/include
)

target_link_directories(flock PRIVATE
    deps/assimp/lib/Release
)

target_link_libraries(flock PRIVATE glfw Opengl32 assimp-vc143-mt)