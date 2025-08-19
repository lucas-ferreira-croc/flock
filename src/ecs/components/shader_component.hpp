#ifndef SHADER_COMPONENT_HPP
#define SHADER_COMPONENT_HPP

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "render/shader.hpp"

struct ShaderComponent
{
    ShaderComponent(std::string vertexShader = "", std::string fragmentShader = "")
    {
        shader = std::make_shared<Shader>();
        shader->createFromFile(vertexShader, fragmentShader);
    }

    void addUniformVec4(std::string name, glm::vec4 value)
    {
        if(uniformVec4.find(name) == uniformVec4.end())
        {
            uniformVec4[name] = value;
        }

    }

    void updateUniformVec4(std::string name, glm::vec4 value)
    {
        uniformVec4[name] = value;
    }

    void addUniformVec3(std::string name, glm::vec3 value)
    {
        if(uniformVec3.find(name) == uniformVec3.end())
        {
            uniformVec3[name] = value;
        }

    }

    void updateUniformVec3(std::string name, glm::vec3 value)
    {
        uniformVec3[name] = value;
    }

    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, glm::vec4> uniformVec4;
    std::unordered_map<std::string, glm::vec3> uniformVec3;
};

#endif