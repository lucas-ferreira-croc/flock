#ifndef EDITMODE_SYSTEM_HPP
#define EDITMODE_SYSTEM_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "ecs/ecs.hpp"
#include "ecs/components/vertex_edit_stub_component.hpp"
#include "ecs/components/edit_component.hpp"
#include "ecs/components/id_component.hpp"
#include "ecs/components/mesh_component.hpp"


class EditModeSystem : public System
{
public:
    EditModeSystem()
    {
        requireComponent<EditComponent>();
    }

    void Update(std::vector<Entity>& entities)
    {

        for (auto& parent : getSystemEntities())
        {
            auto& parentTransform = parent.getComponent<TransformComponent>();
            auto& parentMesh = parent.getComponent<MeshComponent>();
            auto& mesh = parentMesh.model->getMeshes()[0];
            auto& parentVertices = mesh.vertices;
            auto& stubEntities = parent.getComponent<EditComponent>().mVertexStubs;

            if(parent.getComponent<EditComponent>().mCurrentEditType == EditType::VERTEX)
            {
                Entity* editingStub = nullptr;

                for (auto& stubEntity : stubEntities)
                {
                    auto& stub = stubEntity.getComponent<VertexEditStubComponent>();
                    if (stub.mEdit)
                    {
                        editingStub = &stubEntity;
                        break;
                    }
                }

                for (auto& stubEntity : stubEntities)
                {
                    if (&stubEntity == editingStub)
                        continue;

                    auto& stub = stubEntity.getComponent<VertexEditStubComponent>();
                    glm::vec3 worldPos = stub.mLastPosition + parentTransform.position;
                    stubEntity.getComponent<TransformComponent>().position = worldPos;
                }

                if (!editingStub)
                {
                    continue;
                }

                auto& stubTransform = editingStub->getComponent<TransformComponent>();
                auto& stub = editingStub->getComponent<VertexEditStubComponent>();
                int vertexId = stub.mVertexID;

                glm::vec3 worldPos = stubTransform.position;
                glm::vec3 localPos = worldPos - parentTransform.position;

                parentVertices[vertexId].position = localPos;
                mesh.updateVertexPosition(vertexId);

                for (auto& vId : stub.mAssociatedVertices)
                {
                    parentVertices[vId].position = localPos;
                    mesh.updateVertexPosition(vId);
                }

                stub.mLastPosition = localPos;
            }
        }   
    }

    void UpdateVertexEdit(TransformComponent& parentTransform,
                          MeshComponent& meshComponent,
                          Mesh& mesh,
                          std::vector<Vertex> parentVertices,
                          std::vector<Entity>& stubEntities)
    {
    }
};

#endif