#ifndef SHADER_COMPONENT_HPP
#define SHADER_COMPONENT_HPP

#include <string>
#include <memory>

#include "render/shader.hpp"

struct ShaderComponent
{
    ShaderComponent(std::string vertexShader = "", std::string fragmentShader = "")
    {
        shader = std::make_shared<Shader>();
        shader->createFromFile(vertexShader, fragmentShader);
    }

    std::shared_ptr<Shader> shader;
};

#endif