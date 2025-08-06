#ifndef RIGID_BODY_COMPONENT_HPP
#define RIGID_BODY_COMPONENT_HPP

#include <glm/glm.hpp>

struct  RigidBodyComponent
{
    glm::vec3 velocity;

    RigidBodyComponent(glm::vec3 velocity = glm::vec3(1.0f, 1.0f, 1.0f))
    {
        this->velocity = velocity;
    }
};

#endif