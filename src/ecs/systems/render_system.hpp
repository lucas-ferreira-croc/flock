#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "ecs/ecs.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/mesh_component.hpp"

class RenderSystem : public System
{
public:
    RenderSystem()
    {
        requireComponent<MeshComponent>();
        requireComponent<TransformComponent>();
    }

    void Update(Shader& shader, glm::mat4 projection, glm::mat4 lookAt)
    {
        for(auto& entity : getSystemEntities())
        {
            auto& mesh = entity.getComponent<MeshComponent>();
            auto& transform = entity.getComponent<TransformComponent>();

            glm::mat4 wvp(1.0f);
            glm::mat4 modelTransform(1.0f);

            modelTransform = glm::scale(modelTransform, transform.scale);
            //modelTransform = glm::rotate(modelTransform, transform.rotation);
            modelTransform = glm::translate(modelTransform, transform.position);
            
            wvp = projection * lookAt * modelTransform;
            shader.setMat4("wvp", wvp);
            
            shader.bind();
            //shader.setMat4("wvp", wvp);

            mesh.model->render(shader);
        }
    }
};
#endif