#ifndef PHYSICS_SYSTEM_ECS_HPP
#define PHYSICS_SYSTEM_ECS_HPP

#include "ecs/ecs.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"
#include "ecs/components/id_component.hpp"

#include "../src/physics/world.hpp"
#include "log/logger.hpp"

#include "physics/physics_system.hpp"
#include "physics/particle.hpp"
#include "physics/rigidbody_volume.hpp"

enum class RigidbodyType
{
    PARTICLE = 1,
    SPHERE,
    BOX
};

class PhysicsSystemECS : public System
{
public:
    PhysicsSystemECS()
    {
        requireComponent<TransformComponent>();
        requireComponent<IDComponent>();
        requireComponent<RigidBodyComponent>();
        
        physicsSystem = Physics::PhysicsSystem();
        physicsSystem.ImpulseIteration = 20;
        physicsSystem.DoLinearProjection = true;
        physicsSystem.LinearProjectionPercent = 0.450f;
        physicsSystem.PenetrationSlack = 0.010f;
        physicsSystem.ClearRigidbodys();
        physicsSystem.ClearCloths();
        physicsSystem.ClearConstraints();
        physicsSystem.ClearSprings();
    }

    void addRigidBodyBox(glm::vec3 position, glm::vec3 orientation, glm::vec3 size, float mass, std::string id, float restitution = 0.5f)
    {
        Physics::RigidbodyVolume rigidBody;

        rigidBody.type = 3;
        rigidBody.box.size = Physics::vec3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
        rigidBody.position = Physics::vec3(position.x, position.y, position.z);
        rigidBody.orientation = Physics::vec3(orientation.x, orientation.y, orientation.z);
        rigidBody.mass = mass;
        rigidBody.restitution = restitution;
        bodies[id] = rigidBody;

        for(auto& body: bodies)
        {
            body.second.SynchCollisionVolumes();
        }

        physicsSystem.AddRigidbody(&bodies[id]);
    }

    void addRigidBodySphere(glm::vec3 position, float mass, float radius, std::string id, float restitution = 0.5f)
    {
        Physics::RigidbodyVolume rigidBody;

        rigidBody.type = 2;
        rigidBody.sphere.radius = radius;
        rigidBody.position = Physics::vec3(position.x, position.y, position.z);
        rigidBody.mass = mass;
        rigidBody.restitution = restitution;
        bodies[id] = rigidBody;

        for(auto& body: bodies)
        {
            body.second.SynchCollisionVolumes();
        }

        physicsSystem.AddRigidbody(&bodies[id]);
    }
    
    void Update(float deltaTime)
    {
        physicsSystem.Update(deltaTime);
    }   

    std::map<std::string, Physics::RigidbodyVolume> bodies;
private:
    Physics::PhysicsSystem physicsSystem;
    Physics::RigidbodyVolume groundBox;
    
   
};

#endif