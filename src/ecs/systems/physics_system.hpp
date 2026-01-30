#ifndef PHYSICS_SYSTEM_ECS_HPP
#define PHYSICS_SYSTEM_ECS_HPP

#include "ecs/ecs.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"

#include "../src/physics/world.hpp"
#include "log/logger.hpp"

#include "physics/physics_system.hpp"
#include "physics/particle.hpp"
#include "physics/rigidbody_volume.hpp"

class PhysicsSystemECS : public System
{
public:
    PhysicsSystemECS()
    {
        requireComponent<TransformComponent>();
        
        physicsSystem = Physics::PhysicsSystem();
        physicsSystem.ImpulseIteration = 20;

    }

    void Update(float deltaTime)
    {
        int bodiesSize = getSystemEntities().size();
    }   
private:
    Physics::PhysicsSystem physicsSystem;
    std::vector<Physics::RigidbodyVolume> bodies;
    Physics::RigidbodyVolume groundBox;
    
   
};

#endif