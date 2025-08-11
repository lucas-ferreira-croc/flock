#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

enum class MeshType
{
    BOX = 0,
    PLANE,
    SPHERE,
    PYRAMID,
};

struct MeshComponent
{

    MeshComponent(MeshType meshType = MeshType::BOX)
    {

    }

    // MeshComponent()
    // {
        
    // }
};

#endif