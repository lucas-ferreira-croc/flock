#ifndef PHYSICS_SYSTEM_HPP
#define PHYSICS_SYSTEM_HPP

#include "ecs/ecs.hpp"
#include "../components/physics_shape_component.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"

#include "../src/physics/world.hpp"
#include "log/logger.hpp"

class PhysicsSystem : public System
{
public:
    PhysicsSystem(float gravity)
    {
        Logger::warning("Creating physics system");
        world = std::make_shared<World>(gravity);
        requireComponent<TransformComponent>();
        //requireComponent<RigidBodyComponent>();
        requireComponent<PhysicsShapeComponent>();
    }

    void AddBody(Entity& entity)
    {
        if(entity.hasComponent<PhysicsShapeComponent>() && entity.hasComponent<TransformComponent>());
            auto shapeComponent = entity.getComponent<PhysicsShapeComponent>();
            auto transformComponent = entity.getComponent<TransformComponent>();

            auto physicsShape = shapeToPhysics(shapeComponent.type);

            std::shared_ptr<Body> body = std::make_shared<Body>(physicsShape, 
                transformComponent.position.x, transformComponent.position.y, transformComponent.position.z, 
                shapeComponent.mass);
            world->AddBody(body);
    }

    void addForce(const glm::vec3 force)
    {
        world->AddForce(force);
    }

    void AddTorque(float torque)
    {
        world->AddTorque(torque);
    }   

    void Update(float deltaTime)
    {
        world->Update(deltaTime);
        auto bodies = world->GetBodies();

        int i = 0;
        for(auto& entity : getSystemEntities())
        {
            auto body = bodies[i++];
            auto& transform = entity.getComponent<TransformComponent>();

            transform.position = body->position;
        }
    }

    std::shared_ptr<World> world;

private:
    std::shared_ptr<Shape> shapeToPhysics(PhysicsShapeComponent shapeComponent)
    {
        switch(shapeComponent.type)
        {
            case PhysicsShapeType::BOX:
            {
                std::shared_ptr<BoxShape> body = std::make_shared<BoxShape>(shapeComponent.size, shapeComponent.size, shapeComponent.size);
                return body->Clone();
            }

            case PhysicsShapeType::PLANE:
            {
                std::shared_ptr<BoxShape> body = std::make_shared<BoxShape>(shapeComponent.size, shapeComponent.size / 100.0f, shapeComponent.size);
                return body->Clone();
            }

            case PhysicsShapeType::SPHERE:
            {
                std::shared_ptr<CircleShape> body = std::make_shared<CircleShape>(shapeComponent.size);
                return body->Clone();
            }

            default:
            {
                std::shared_ptr<BoxShape> body = std::make_shared<BoxShape>(shapeComponent.size, shapeComponent.size, shapeComponent.size);
                return body->Clone();
            }
        }
        std::shared_ptr<BoxShape> body = std::make_shared<BoxShape>(shapeComponent.size, shapeComponent.size, shapeComponent.size);
        return body->Clone();
    }
};

#endif