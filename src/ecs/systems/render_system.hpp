#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "render/display.hpp"
#include "ecs/ecs.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/mesh_component.hpp"
#include "ecs/components/shader_component.hpp"
#include "ecs/components/physics_shape_component.hpp"
#include "ecs/components/material_component.hpp"
#include "ecs/components/cubemap_component.hpp"

class RenderSystem : public System
{
public:
    RenderSystem()
    {
        //requireComponent<MeshComponent>();
        requireComponent<TransformComponent>();
        requireComponent<ShaderComponent>();

        std::string vsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\v.glsl";
        std::string fsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\debug_f.glsl";

        debugShader = std::make_shared<Shader>();
        debugShader->createFromFile(vsFilename, fsFilename);
    }

    void Update(glm::mat4 projection, std::shared_ptr<Camera> camera, Display& display)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, display.framebuffer);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        display.clearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        display.clear();

        for(auto& entity : getSystemEntities())
        {            
            auto& transform = entity.getComponent<TransformComponent>();
            auto& shaderComponent = entity.getComponent<ShaderComponent>();

            glm::mat4 wvp(1.0f);
            glm::mat4 modelTransform(1.0f);
                        
            auto shader = shaderComponent.shader;
            shader->bind();
            

            if(entity.hasComponent<CubemapComponent>())
            {
                auto cubemapComponent = entity.getComponent<CubemapComponent>();
                glDepthFunc(GL_LEQUAL);
                glDepthMask(GL_FALSE); // não escreve no depth buffer
                
                shader->setMat4("view", glm::mat3(camera->getLookAt()));
                shader->setMat4("projection", projection);

                glBindVertexArray(cubemapComponent.VAO);
                cubemapComponent.cubemap->use(GL_TEXTURE0);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);
                continue;
            }

            modelTransform = glm::translate(modelTransform, transform.position);
            modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            modelTransform = glm::scale(modelTransform, transform.scale);

            shader->setMat4("projection", projection);
            shader->setMat4("view", camera->getLookAt());
            shader->setMat4("model", modelTransform);

            bool hasLightPosition = shaderComponent.uniformVec3.find("light.position") != shaderComponent.uniformVec3.end();
            bool hasLightDirection = shaderComponent.uniformVec3.find("light.direction") != shaderComponent.uniformVec3.end();
            if(hasLightDirection || hasLightPosition)
            {
                if(hasLightPosition)
                {
                    for(auto& uniform : shaderComponent.uniformFloat)
                    {
                        shader->setFloat(uniform.first, uniform.second);
                    }
                }

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

            for(auto& uniform : shaderComponent.uniformVec3)
            {
                // camera position for reflection, refactor later
                shader->setFloat3("cameraPos", camera->getPosition());
            }
   
   
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
        display.renderFramebuffer();
    }
    
    bool debugMode = true;
private:
    std::shared_ptr<Shader> debugShader;
};
#endif