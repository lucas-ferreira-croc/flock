#ifndef COLLISION_EVENT_HPP
#define COLLISION_EVENT_HPP

#include "ecs/ecs.hpp"
#include "event_bus/event.hpp"

class CollisionEvent : public Event 
{
public:
    Entity a;
    Entity b;
    CollisionEvent(Entity a, Entity b) : a(a), b(b) {}
}


#endif