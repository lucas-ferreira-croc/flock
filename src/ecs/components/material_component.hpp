#ifndef MATERIAL_COMPONENT_HPP
#define MATERIAL_COMPONENT_HPP

#include "glm/glm.hpp"

struct MaterialComponent
{
    MaterialComponent(glm::vec3 ambient = glm::vec3(1.0f), glm::vec3 diffuse = glm::vec3(1.0f), glm::vec3 specular = glm::vec3(1.0f), float shininess = 32.0f)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->shininess = shininess;
    }

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

#endif