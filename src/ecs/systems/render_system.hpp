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
        
        lightDirection = glm::vec3(0.0f, 4.0f, 1.0f);
    }

void Update(glm::mat4 projection, std::shared_ptr<Camera> camera, Display& display)
{
    // ====================
    // 1. SHADOW PASS - LUZ DIRECIONAL
    // ====================
    float near_plane = 0.1f, far_plane = 15.0f;
    
    // Para luz direcional: lightDirection aponta PARA ONDE a luz vai
    // Então a câmera da luz deve estar na direção OPOSTA
    // lightDirection = normalize(-2, -4, -2), então a luz vem de cima/direita/frente
    
    // Posicionar a "câmera da luz" longe, olhando para o centro da cena
    glm::vec3 lightPos = lightDirection; 
    glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Centro aproximado da sua cena
    
    // Projeção ORTOGRÁFICA para luz direcional
    // Ajustar área para cobrir exatamente sua cena (objetos entre y=-5 e y=0 aprox)
    float shadowArea = 10.0f;
    glm::mat4 lightProjection = glm::ortho(
        -shadowArea, shadowArea,    
        -shadowArea, shadowArea,    
        near_plane, far_plane       
    );
    
    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    
    // Configurar shader de depth
    display._depthbufferShader->bind();
    display._depthbufferShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    
    // Configurar framebuffer de sombras
    glViewport(0, 0, display.SHADOW_WIDTH, display.SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, display.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    // DESABILITAR culling durante shadow pass para evitar problemas
    // com objetos que têm normais invertidas ou geometria complexa
    //glDisable(GL_CULL_FACE);
    
    // Renderizar todos os objetos da cena no depth buffer
    for(auto entity : getSystemEntities())
    {
        // Pular skybox - não projeta sombra
        if(entity.hasComponent<CubemapComponent>()) continue;
        if(!entity.hasComponent<MeshComponent>()) continue;
        
        auto& transform = entity.getComponent<TransformComponent>();           
        
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::translate(modelTransform, transform.position);
        modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelTransform = glm::scale(modelTransform, transform.scale);
 
        display._depthbufferShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        display._depthbufferShader->setMat4("model", modelTransform);
        
        auto& mesh = entity.getComponent<MeshComponent>();
        mesh.model->render(*display._depthbufferShader);
    }
    
    // Restaurar culling normal
    //glCullFace(GL_BACK);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // ====================
    // 2. RENDER PASS NORMAL
    // ====================
    glViewport(0, 0, display.getBufferWidth(), display.getBufferHeight());
    //display.clearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    display.clear();
    
    // Renderizar a cena com sombras
    renderPass(projection, camera, display, RenderPass::RENDER, lightSpaceMatrix);
    
    // ====================
    // 3. DEBUG DO DEPTH BUFFER
    // ====================
    if(debugMode)
    {
        int debugSize = 400;
        glViewport(display.getBufferWidth() - debugSize, 0, debugSize, debugSize);
        
        display.depthDebugShader->bind();
        display.depthDebugShader->setFloat("near_plane", near_plane);
        display.depthDebugShader->setFloat("far_plane", far_plane);
        display.depthDebugShader->setInt("depthMap", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, display.depthMap);
        
        // Desabilitar depth test para garantir que o quad apareça
        glDisable(GL_DEPTH_TEST);
        renderQuad();
        glEnable(GL_DEPTH_TEST);
        
        glViewport(0, 0, display.getBufferWidth(), display.getBufferHeight());
    }
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

void renderPass(glm::mat4 projection, std::shared_ptr<Camera> camera, Display& display, 
                RenderPass pass, glm::mat4 lightSpaceMatrix)
{
    for(auto& entity : getSystemEntities())
    {            
        auto& transform = entity.getComponent<TransformComponent>();
        auto& shaderComponent = entity.getComponent<ShaderComponent>();

        auto shader = shaderComponent.shader;
        shader->bind();
        
        // Skybox - renderizar sem sombras
        if(entity.hasComponent<CubemapComponent>())
        {
            auto cubemapComponent = entity.getComponent<CubemapComponent>();
            glDepthFunc(GL_LEQUAL);
            glDepthMask(GL_FALSE);
            
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

        // Calcular transform do modelo
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::translate(modelTransform, transform.position);
        modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelTransform = glm::scale(modelTransform, transform.scale);

        // Configurar matrizes básicas
        shader->setMat4("projection", projection);
        shader->setMat4("view", camera->getLookAt());
        shader->setMat4("model", modelTransform);
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        
        // IMPORTANTE: Shadow map vai para texture unit 1
        // Texture unit 0 fica para as texturas do modelo

        shader->setFloat3("viewPos", camera->getPosition());

        // Configurar iluminação
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
        }

        for(auto& uniform : shaderComponent.uniformVec4)
        {
            shader->setFloat4(uniform.first, uniform.second);
        }

        for(auto& uniform : shaderComponent.uniformVec3)
        {
            shader->setFloat3("cameraPos", camera->getPosition());
        }

        // Material
        if(entity.hasComponent<MaterialComponent>())
        {
            auto& material = entity.getComponent<MaterialComponent>();
            shader->setFloat3("material.ambient", material.ambient);
            shader->setFloat3("material.diffuse", material.diffuse);
            shader->setFloat3("material.specular", material.specular);
            shader->setFloat("material.shininess", material.shininess);
        }

        // Renderizar mesh
        if(entity.hasComponent<MeshComponent>())
        {
            shader->setInt("shadowMap", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, display.depthMap);
            
            auto& mesh = entity.getComponent<MeshComponent>();
            mesh.model->render(*shader);
        }


    }
}

    bool debugMode = true;
private:
    glm::vec3 lightDirection;
    glm::mat4 lightView;
    glm::mat4 lightProjection;
    std::shared_ptr<Shader> debugShader;
};
#endif