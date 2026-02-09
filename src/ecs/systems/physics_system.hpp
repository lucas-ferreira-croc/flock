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

struct PickingInfo
{
    Physics::vec2 viewportPoint;
    Physics::vec2 viewportOrigin;
    Physics::vec2 viewportSize;
    Physics::mat4 view;
    Physics::mat4 projection;
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
    
    void Update(float deltaTime, bool clicked = false, std::shared_ptr<PickingInfo> pickingInfo = std::shared_ptr<PickingInfo>())
    {
        physicsSystem.Update(deltaTime);
        if(clicked)
        {   
            Physics::vec3 nearPoint(pickingInfo->viewportPoint.x, pickingInfo->viewportPoint.y, 0.0f);
            Physics::vec3 farPoint(pickingInfo->viewportPoint.x, pickingInfo->viewportPoint.y, 1.0f);

            Physics::vec3 pNear = Physics::Unproject(nearPoint, pickingInfo->viewportOrigin, pickingInfo->viewportSize, pickingInfo->view, pickingInfo->projection);
            Physics::vec3 pFar = Physics::Unproject(farPoint, pickingInfo->viewportOrigin, pickingInfo->viewportSize, pickingInfo->view, pickingInfo->projection);

            Physics::vec3 normal = Physics::Normalized(pFar - pNear);
            Physics::vec3 origin = pNear;

            Physics::Ray screenRay(origin, normal);
            screenRay.NormalizeDirection();

            std::vector<Physics::RaycastResult> allCasts;
            allCasts.resize(bodies.size());

            int i = 0;
            std::vector<std::string> ids;
            ids.resize(bodies.size());

            for(auto body : bodies)
            {
                if(body.second.type == RIGIDBODY_TYPE_SPHERE)
                {
                    Raycast(body.second.sphere, screenRay, &allCasts[i]);
                    ids.at(i) = body.first;
                    i++;
                }
                else if(body.second.type == RIGIDBODY_TYPE_BOX)
                {
                    Raycast(body.second.sphere, screenRay, &allCasts[i]);
                    ids.at(i) = body.first;
                    i++;
                }
            }

            Physics::RaycastResult raycastResult;
            Physics::ResetRaycastResult(&raycastResult);
            int manipulating = -1;
            int wasManipulating = manipulating;
            std::string id = "";
            for(int i = 0; i < allCasts.size(); i++)
            {
                if(allCasts[i].hit)
                {
                    if(!raycastResult.hit || allCasts[i].t < raycastResult.t)
                    {
                        raycastResult = allCasts[i];
                        manipulating = i;
                        id = ids.at(i);
                        std::cout << id << "\n";
                    }
                }
            }

            for(auto entity : getSystemEntities())
            {
                if(entity.getComponent<IDComponent>()._name == id)
                {
                    if(previousPicked != id)
                    {
                        for(auto& entity : getSystemEntities())
                        {
                            if(entity.getComponent<IDComponent>()._name == previousPicked)
                            {
                                entity.getComponent<IDComponent>().isPicked = false;
                            }
                        }
                    }
                    previousPicked = id;
                    entity.getComponent<IDComponent>().isPicked = true;
                }
            }
        }
    }   

    std::map<std::string, Physics::RigidbodyVolume> bodies;
private:
    Physics::PhysicsSystem physicsSystem;
    Physics::RigidbodyVolume groundBox;
    std::string previousPicked;
   
};

#endif