#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP

#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"
#include "ecs/ecs.hpp"

class MovementSystem : public System 
{
public: 
    MovementSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<RigidBodyComponent>();
    }
    void Update(double deltaTime)
    {
        for(auto entity : getSystemEntities())
        {
            auto& transform = entity.getComponent<TransformComponent>();
            auto rigidBody = entity.getComponent<RigidBodyComponent>();

            transform.position.x += rigidBody.velocity.x * deltaTime;
            transform.position.y += rigidBody.velocity.y * deltaTime;
            transform.position.z += rigidBody.velocity.z * deltaTime;
        }
    }
};

#endif