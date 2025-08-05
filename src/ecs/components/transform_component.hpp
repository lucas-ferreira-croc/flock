#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include <glm/glm.hpp>

struct TransformComponent
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    TransformComponent(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f)){
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
    }
};

#endif