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
#include "events/rigidbody_changed_event.hpp"


class EditModeSystem : public System
{
public:
    EditModeSystem()
    {
        requireComponent<EditComponent>();
    }

    void Update(std::vector<Entity>& entities, std::unique_ptr<EventBus>& eventBus)
    {
        std::pair<bool, std::string> editedEntity;
        editedEntity.first = false;
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
                editedEntity.first = true;
                editedEntity.second = parent.getComponent<IDComponent>()._name;
            }
            else if(parent.getComponent<EditComponent>().mCurrentEditType == EditType::EDGE)
            {
                std::vector<Entity*> editingStub;
                for (auto& stubEntity : stubEntities)
                {
                    auto& stub = stubEntity.getComponent<VertexEditStubComponent>();
                    if (stub.mEdit)
                    {
                        editingStub.push_back(&stubEntity);
                    }
                }
                
                if (editingStub.size() != 2)
                {
                    continue;
                }
                for(int i = 0; i < editingStub.size(); i++)
                {

                    auto& stubTransform = editingStub[i]->getComponent<TransformComponent>();
                    auto& stub = editingStub[i]->getComponent<VertexEditStubComponent>();
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
                editedEntity.first = true;
                editedEntity.second = parent.getComponent<IDComponent>()._name;
            }
            else if(parent.getComponent<EditComponent>().mCurrentEditType == EditType::FACE)
            {
                 std::vector<Entity*> editingStub;
                for (auto& stubEntity : stubEntities)
                {
                    auto& stub = stubEntity.getComponent<VertexEditStubComponent>();
                    if (stub.mEdit)
                    {
                        editingStub.push_back(&stubEntity);
                    }
                }
                
                if (editingStub.size() != 3)
                {
                    continue;
                }
                for(int i = 0; i < editingStub.size(); i++)
                {

                    auto& stubTransform = editingStub[i]->getComponent<TransformComponent>();
                    auto& stub = editingStub[i]->getComponent<VertexEditStubComponent>();
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
                editedEntity.first = true;
                editedEntity.second = parent.getComponent<IDComponent>()._name;
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