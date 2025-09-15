#ifndef DEBUG_MESH_COMPONENT_HPP
#define DEBUG_MESH_COMPONENT_HPP

#include <glm/glm.hpp>
#include <memory>
#include "render/mesh.hpp"

enum class DebugMeshType
{
    BOX = 0,
    PLANE,
    SPHERE,
    CONE,
    CYLINDER
};


struct DebugMeshComponent
{
    DebugMeshComponent(DebugMeshType debugMeshType = DebugMeshType::BOX, glm::vec3 position = glm::vec3(0.0f), float width = 1.0f, float height = 1.0f, float depth = 1.0f)
    {
        switch(debugMeshType)
        {
            case DebugMeshType::BOX:
            {
                unsigned int indices[] = {
                    0, 1, 2,  2, 3, 0, // traseira (z - d)
                    1, 6, 2,  6, 4, 2, // direita (x + w)
                    6, 7, 4,  7, 5, 4, // frente  (z + d)
                    7, 0, 5,  0, 3, 5, // esquerda (x - w)
                    3, 2, 5,  2, 4, 5, // topo    (y + h)
                    7, 6, 0,  6, 1, 0  // base    (y - h)
                };

    
    
                float w = width * 0.5f;
                float h = height * 0.5f;
                float d = depth * 0.5f;
    
                float vertices[] = {
                    position.x - w, position.y - h, position.z - d, // 0
                    position.x + w, position.y - h, position.z - d, // 1
                    position.x + w, position.y + h, position.z - d, // 2
                    position.x - w, position.y + h, position.z - d, // 3
                    position.x + w, position.y + h, position.z + d, // 4
                    position.x - w, position.y + h, position.z + d, // 5
                    position.x + w, position.y - h, position.z + d, // 6
                    position.x - w, position.y - h, position.z + d, // 7
                };


                debugMesh = std::make_shared<DebugMesh>(vertices, 24, indices, 36);
            }    
        }
    }

    std::shared_ptr<DebugMesh> debugMesh;
};

#endif