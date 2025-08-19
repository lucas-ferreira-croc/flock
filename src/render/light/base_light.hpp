#ifndef BASE_LIGHT_HPP
#define BASE_LIGHT_HPP

#include "glm/glm.hpp"

struct BaseLight
{
    BaseLight(glm::vec3 positon, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);

    glm::vec3 position;
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

};

#endif