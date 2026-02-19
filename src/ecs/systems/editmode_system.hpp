#ifndef EDITMODE_SYSTEM_HPP
#define EDITMODE_SYSTEM_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "ecs/ecs.hpp"
#include "ecs/components/edit_stub_component.hpp"
#include "ecs/components/edit_component.hpp"
#include "ecs/components/id_component.hpp"
#include "ecs/components/mesh_component.hpp"


class EditModeSystem : public System
{
public:
    EditModeSystem()
    {
        requireComponent<VertexEditStubComponent>();
    }

    void Update(std::vector<Entity>& entities)
    {
        parents.clear();

        for(int i = 0; i < entities.size(); i++)
        {
            if(entities.at(i).hasComponent<EditComponent>())
            {
                parents.push_back(i);
            }
        }

        for (auto parentIdx : parents)
        {
            auto& parent = entities[parentIdx];
            auto& parentTransform = parent.getComponent<TransformComponent>();
            auto& stub = parent.getComponent<VertexEditStubComponent>();

            glm::vec3 delta = parentTransform.position - stub.mLastPosition;

            if (glm::length(delta) < 1e-6f)
                continue;

            for (auto& entity : getSystemEntities())
            {
                entity.getComponent<TransformComponent>().position += delta;
            }

            stub.mLastPosition = parentTransform.position;
        }

        std::pair<int, glm::vec3> idPosition = { -1, glm::vec3(0.0f) };

        for (auto& entity : getSystemEntities())
        {
            auto& editStub = entity.getComponent<VertexEditStubComponent>();
            if (editStub.mEdit)
            {
                idPosition = { editStub.mVertexID, entity.getComponent<TransformComponent>().position };
                break;
            }
        }

        if (idPosition.first < 0)
        {
            return;
        }

        for (auto parentIdx : parents)
        {
            auto& parent = entities[parentIdx];
            auto& parentTransform = parent.getComponent<TransformComponent>();
            auto& parentMesh = parent.getComponent<MeshComponent>();

            for(auto& vertex : parentMesh.model->getMeshes().at(0).vertices)
            {
                if(vertex.id == idPosition.first)
                {
                    std::vector<Entity>& stubbedEntities = getSystemEntities();
                    int i = 0;
                    for(auto& stub : stubbedEntities)
                    {
                        if(stub.getComponent<VertexEditStubComponent>().mVertexID == vertex.id)
                        {
                            break;
                        }
                        i++;
                    }

                    auto& entity = stubbedEntities.at(i);
                    glm::vec3 worldPos = entity.getComponent<TransformComponent>().position;
                    glm::vec3 localPos = worldPos - parentTransform.position;
                    glm::vec3 basePos = vertex.position;
                    auto associatedVertices = entity.getComponent<VertexEditStubComponent>().associatedVertices;

                    int j = 0;
                    auto& parentVertices = parentMesh.model->getMeshes()[0].vertices;
                    
                    parentVertices.at(idPosition.first).position = localPos;
                    parentMesh.model->getMeshes()[0].updateVertexPosition(idPosition.first);
                    for(auto vId : associatedVertices)
                    {
                        parentVertices.at(vId).position = localPos;
                        parentMesh.model->getMeshes()[0].updateVertexPosition(vId);
                    }
                }
            }
        }

        parents.clear();
    }
private:
    std::vector<int> parents;
};

#endif