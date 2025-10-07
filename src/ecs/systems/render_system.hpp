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

enum class RenderPass 
{
    RENDER = 0,
    SHADOW
};

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
        float near_plane = 1.0f, far_plane = 7.5f;
        //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);

        lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -2.0f),
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f)); 
    }

    void Update(glm::mat4 projection, std::shared_ptr<Camera> camera, Display& display)
    {
        display.clearColor(0.1f, 0.1f, 0.1f, 1.0f); 
        display.clear();
        ///
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 30.0f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -2.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        display._depthbufferShader->bind();
        display._depthbufferShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, display.SHADOW_WIDTH, display.SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, display.depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            for(auto entity : getSystemEntities())
            {
                auto& shaderComponent = entity.getComponent<ShaderComponent>();
                auto& transform = entity.getComponent<TransformComponent>();           
                
                glm::mat4 modelTransform(1.0f);
                modelTransform = glm::translate(modelTransform, transform.position);
                modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                modelTransform = glm::scale(modelTransform, transform.scale);

                auto shader = shaderComponent.shader;
                display._depthbufferShader->bind();
                display._depthbufferShader->setMat4("model", modelTransform);

                if(entity.hasComponent<MeshComponent>())
                {
                    auto& mesh = entity.getComponent<MeshComponent>();
                    mesh.model->render(*display._depthbufferShader);
                }
            }
            //renderPass(projection, camera, display, RenderPass::RENDER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, display.getBufferWidth(), display.getBufferHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display.depthDebugShader->bind();
        display.depthDebugShader->setFloat("near_plane", near_plane);
        display.depthDebugShader->setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, display.depthMap);
        renderQuad();
        ///

        //display.renderDepthBuffer();

        //renderPass(lightProjection, camera, display, RenderPass::SHADOW);
       
        // glBindFramebuffer(GL_FRAMEBUFFER, display.framebuffer);
        // glEnable(GL_DEPTH_TEST);
        // glDisable(GL_BLEND);
        // glViewport(0, 0, display.getBufferWidth(), display.getBufferHeight());
        // display.clear();


        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //renderPass(projection, camera, display, RenderPass::RENDER);
        //glBindTexture(GL_TEXTURE_2D, display.depthMap);
        
        //display.renderFramebuffer();
    }
    
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void renderPass(glm::mat4 projection, std::shared_ptr<Camera> camera, Display& display, RenderPass pass)
    {
        if(pass == RenderPass::RENDER)
        {
            display.renderDepthBufferDebug();
        }

        if(pass == RenderPass::SHADOW)
        {
            glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
            display._depthbufferShader->bind();
            display._depthbufferShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        }

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
            if(pass == RenderPass::RENDER)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, display.depthMap);
                
                glm::mat4 lightSpaceMatrix = lightProjection * lightView;
                shader->setMat4("view", camera->getLookAt());
                shader->setInt("shadowMap", 0);
                shader->setMat4("model", modelTransform);
            }
            
            else if(pass == RenderPass::SHADOW)
            {
                display._depthbufferShader->bind();
                display._depthbufferShader->setMat4("model", modelTransform);

                shader->bind();
                shader->setMat4("model", modelTransform);
            }

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
    }
    bool debugMode = true;
private:
    glm::mat4 lightView;
    glm::mat4 lightProjection;
    std::shared_ptr<Shader> debugShader;
};
#endif