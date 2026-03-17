#ifndef COLLISION_EVENT_HPP
#define COLLISION_EVENT_HPP

#include <glm/glm.hpp>
#include "ecs/ecs.hpp"
#include "event_bus/event.hpp"

class RigidBodyChangedEvent : public Event 
{
public:
    Entity mEntity;
    std::string mId;
    glm::vec3 mHalfExtents;
    float mRadius;
    RigidBodyChangedEvent(Entity entity, std::string id, glm::vec3 extents = glm::vec3(1.0f), float radius = 1.0f)
        :  mEntity(entity), mId(id), mHalfExtents(extents), mRadius(radius)
    {

    }
};


#endif