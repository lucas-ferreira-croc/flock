#ifndef EDIT_COMPONENT_HPP
#define EDIT_COMPONENT_HPP

#include "ecs/components/transform_component.hpp"
#include "ecs/components/mesh_component.hpp"
#include "ecs/components/shader_component.hpp"
#include "ecs/components/material_component.hpp"
#include "ecs/components/id_component.hpp"
#include "ecs/components/tag_component.hpp"
#include "ecs/components/vertex_edit_stub_component.hpp"
#include "ecs/ecs.hpp"

// #include "log/logger.hpp"

enum class EditType
{   
    NONE = 0,
    VERTEX,
    EDGE,
    FACE
};

struct EditComponent
{
    EditComponent(){}
    EditComponent(std::shared_ptr<Registry> registry 
    , std::vector<Entity>& entities , MeshComponent& meshComponent , TransformComponent& transformComponent, IDComponent idComponent, glm::vec3 cameraPosition)
    {
        const float eps = 1e-6f;
        auto equals = [&](const glm::vec3& a, const glm::vec3& b)
        {
            return glm::length(a - b) < eps;
        };

        std::vector<Vertex> verticesPositions;
        std::vector<Vertex> uniqueVertices;
        std::vector<int> ids;
        std::map<int, std::vector<int>> associatedVertices;
        for(auto& mesh : meshComponent.model->getMeshes()) 
        {
            for (auto& v : mesh.vertices)
            {
                bool found = false;
                for (auto& u : uniqueVertices)
                {
                    if (equals(u.position, v.position))
                    {
                        found = true;
                        associatedVertices[u.id].push_back(v.id);
                    }
                }

                if (!found)
                {
                    ids.push_back(v.id);
                    uniqueVertices.push_back(v);
                }
            }
        }

        verticesPositions = std::move(uniqueVertices);

        int i = 0;
        int vertexId = 0;
        for(auto p : verticesPositions)
        {
            Entity vertice = registry->createEntity();
            //vertice.addComponent<MeshComponent>("C:\\dev\\shader\\flock\\assets\\models\\cube.obj");
            vertice.addComponent<TransformComponent>(p.position + transformComponent.position, glm::vec3(.05f, .05f, .05f) , glm::vec3(0.0f, 0.0f, 0.0f));
            vertice.addComponent<MaterialComponent>(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 8.0f);
            vertice.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
            vertice.getComponent<ShaderComponent>().addUniformVec3("cameraPos", cameraPosition);
            vertice.addComponent<TagComponent>(TagType::EditModeEntity);
            std::string id = "v" + std::to_string(i++);
            vertice.addComponent<VertexEditStubComponent>(p.position, ids.at(vertexId));
            vertice.getComponent<VertexEditStubComponent>().mParent = idComponent._name;

            auto key = ids.at(vertexId);
            std::vector<int> associated;
            auto it = associatedVertices.find(key);
            if(it != associatedVertices.end())
            {
                associated = it->second;
            }
            vertice.getComponent<VertexEditStubComponent>().setAssociatedVertices(associated);
            vertice.getComponent<VertexEditStubComponent>().mParent = idComponent._name;
            vertice.addComponent<IDComponent>(id);
            mVertexStubs.push_back(vertice);
            entities.push_back(vertice);
            vertexId++;
        }

        auto& edges = meshComponent.model->getMeshes()[0].edges;
        mEdgeStubs = edges;

        auto& faces = meshComponent.model->getMeshes()[0].faces;
        mFaceStubs = faces;
    }
    std::string vsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\v.glsl";
    std::string fsColorfilename =  "C:\\dev\\shader\\flock\\assets\\shaders\\f_color.glsl";
    std::string parent;
    std::vector<Entity> mVertexStubs;
    std::vector<Edge> mEdgeStubs;
    std::vector<Face> mFaceStubs;
    EditType mCurrentEditType;
};


#endif