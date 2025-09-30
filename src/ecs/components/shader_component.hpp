#ifndef SHADER_COMPONENT_HPP
#define SHADER_COMPONENT_HPP

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <sstream>

#include "render/shader.hpp"
#include "render/light/directional_light.hpp"
#include "render/light/point_light.hpp"
#include "render/light/spot_light.hpp"

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

    void addUniformFloat(std::string name, float value){
        uniformFloat[name] = value;
    }

    void addUniformInt(std::string name, int value){
        uniformInt[name] = value;
    }

    void setDirectionalLight(DirectionalLight& light)
    {
        directionalLight = light;
        shader->bind();
        shader->setFloat3("directional_light.base.color", light.color);
        shader->setFloat("directional_light.base.ambient", light.ambientIntensity);
        shader->setFloat("directional_light.base.diffuse", light.diffuseIntensity);
        shader->setFloat3("directional_light.direction", light.direction);
    }

    void setPointLights(std::vector<PointLight>& pointLights)
    {
        _pointLights = pointLights;
        shader->bind();
        shader->setInt("point_lights_size", pointLights.size());

        std::stringstream ss;
        for(int i = 0; i < pointLights.size(); i++)
        {
            ss << "point_lights[" << i << "]";
            //std::string name = "pointLights[" + i + ']';
            std::string name = ss.str();
            // base
            shader->setFloat3(name + ".base.color", pointLights[i].color);
            shader->setFloat(name  + ".base.ambient", pointLights[i].ambientIntensity);
            shader->setFloat(name  + ".base.diffuse", pointLights[i].diffuseIntensity);

            // point light
            shader->setFloat3(name + ".position", pointLights[i].position);

            // attenuation
            shader->setFloat(name + ".constant", pointLights[i].attenuation.constant);
            shader->setFloat(name + ".linear", pointLights[i].attenuation.linear);
            shader->setFloat(name + ".quadratic", pointLights[i].attenuation.exp);
            ss.str("");
        }
    }

    void setSpotLights(std::vector<SpotLight>& spotLights)
    {
        _spotLights = spotLights;
        shader->bind();
        shader->setInt("spot_lights_size", spotLights.size());

        std::stringstream ss;
        for(int i = 0; i < spotLights.size(); i++)
        {
            ss << "spot_lights[" << i << "]";
            //std::string name = "pointLights[" + i + ']';
            std::string name = ss.str();
            // base
            shader->setFloat3(name + ".pointLightBase.base.color", spotLights[i].color);
            shader->setFloat(name  + ".pointLightBase.base.ambient", spotLights[i].ambientIntensity);
            shader->setFloat(name  + ".pointLightBase.base.diffuse", spotLights[i].diffuseIntensity);

            // point light
            shader->setFloat3(name + ".pointLightBase.position", spotLights[i].position);

            // attenuation
            shader->setFloat(name + ".pointLightBase.constant", spotLights[i].attenuation.constant);
            shader->setFloat(name + ".pointLightBase.linear", spotLights[i].attenuation.linear);
            shader->setFloat(name + ".pointLightBase.quadratic", spotLights[i].attenuation.exp);

            glm::vec3 normalizedDirection = glm::normalize(spotLights[i].direction);
            shader->setFloat3(name + ".direction", normalizedDirection);
            shader->setFloat(name  + ".cutoff", spotLights[i].cutoff);
            shader->setFloat(name  + ".outerCutOff", spotLights[i].outerCutofff);
            ss.str("");
        }
    }

    void reset()
    {
        shader->reload();
        setDirectionalLight(directionalLight);
        setPointLights(_pointLights);
        setSpotLights(_spotLights);

        shader->bind();
        for(auto uniform : uniformFloat)
        {
            shader->setFloat(uniform.first, uniform.second);
        }

        for(auto uniform : uniformInt)
        {
            shader->setInt(uniform.first, uniform.second);
        }

        for(auto uniform : uniformVec3)
        {
            shader->setFloat3(uniform.first, uniform.second);
        }

        for(auto uniform : uniformVec4)
        {
            shader->setFloat4(uniform.first, uniform.second);
        }
    }
    std::shared_ptr<Shader> shader;
    std::vector<SpotLight> _spotLights;
    std::vector<PointLight> _pointLights;
    DirectionalLight directionalLight;
    std::unordered_map<std::string, glm::vec4> uniformVec4;
    std::unordered_map<std::string, glm::vec3> uniformVec3;
    std::unordered_map<std::string, float> uniformFloat;
    std::unordered_map<std::string, int> uniformInt;
    //DirectionalLight light;
    //std::vector<PointLight> pointLights;
    //std::vector<SpotLight> spotLights;


};

#endif