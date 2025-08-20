#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "ecs/ecs.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/mesh_component.hpp"
#include "ecs/components/shader_component.hpp"
#include "ecs/components/material_component.hpp"

class RenderSystem : public System
{
public:
    RenderSystem()
    {
        //requireComponent<MeshComponent>();
        requireComponent<TransformComponent>();
        requireComponent<ShaderComponent>();
    }

    void Update(glm::mat4 projection, std::shared_ptr<Camera> camera)
    {
        for(auto& entity : getSystemEntities())
        {            
            auto& transform = entity.getComponent<TransformComponent>();
            auto& shaderComponent = entity.getComponent<ShaderComponent>();

            glm::mat4 wvp(1.0f);
            glm::mat4 modelTransform(1.0f);
            
            modelTransform = glm::translate(modelTransform, transform.position);
            modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            modelTransform = glm::scale(modelTransform, transform.scale);
            
            auto shader = shaderComponent.shader;
            shader->bind();
            
            shader->setMat4("projection", projection);
            shader->setMat4("view", camera->getLookAt());
            shader->setMat4("model", modelTransform);


            if(shaderComponent.uniformVec3.find("light.position") != shaderComponent.uniformVec3.end())
            {
                for(auto& uniform : shaderComponent.uniformVec3)
                {
                    shader->setFloat3(uniform.first, uniform.second);
                }    
                shader->setFloat3("viewPos", camera->getPosition());
                //continue;
            }

            for(auto& uniform : shaderComponent.uniformVec4)
            {
                shader->setFloat4(uniform.first, uniform.second);
            }
   
            //shader shader.setMat4("wvp", wvp);
            
            //shader.setMat4("wvp", wvp);

            if(entity.hasComponent<MaterialComponent>())
            {
                auto& material = entity.getComponent<MaterialComponent>();
                shader->setFloat3("material.ambient", material.ambient);
                shader->setFloat3("material.diffuse", material.diffuse);
                shader->setFloat3("material.specular", material.specular);
                shader->setFloat("material.shininess", material.shininess);
            }

            if(entity.hasComponent<MeshComponent>())
            {
                auto& mesh = entity.getComponent<MeshComponent>();
                mesh.model->render(*shader);
            }
        }
    }
};
#endif