#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include <memory>

#include "render/model.hpp"

enum class MeshType
{
    BOX = 0,
    PLANE,
    SPHERE,
    CONE,
    CYLINDER
};

struct MeshComponent
{
    MeshComponent(MeshType meshType = MeshType::BOX)
    {
        std::string path;
        switch(meshType)
        {
            case MeshType::BOX:
                path = "C:\\dev\\shader\\flock\\assets\\models\\cube.fbx";
            break;

            case MeshType::PLANE:
                path = "C:\\dev\\shader\\flock\\assets\\models\\plane.fbx";
                break;
            break;

            case MeshType::SPHERE:
                path = "C:\\dev\\shader\\flock\\assets\\models\\sphere.fbx";
                break;
            break;

            case MeshType::CONE:
                path = "C:\\dev\\shader\\flock\\assets\\models\\cone.fbx";
                break;
            break;

            case MeshType::CYLINDER:
                path = "C:\\dev\\shader\\flock\\assets\\models\\cylinder.fbx";
                break;
            break;

            default:
                Logger::err("Something terrible happened, no mesh selected in mesh component");
                path = "C:\\dev\\shader\\flock\\assets\\models\\cube.fbx";
        }

        model = std::make_shared<Model>(path.c_str());
    }

    MeshComponent(std::string path)
    {
        model = std::make_shared<Model>(path.c_str());
    }

    std::shared_ptr<Model> model;
};

#endif