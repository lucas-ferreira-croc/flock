#ifndef IK_SYSTEM_HPP
#define IK_SYSTEM_HPP

#include "ecs/components/fabrik_component.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/ecs.hpp"

class IKSystem : public System 
{
public:
    IKSystem()
    {
        requireComponent<FABRIKComponent>();
        requireComponent<TransformComponent>();
    }

    void Update(glm::vec3 target)
    {
        for(auto& entity : getSystemEntities())
        {   
            entity.getComponent<FABRIKComponent>().target = target;
            entity.getComponent<FABRIKComponent>().solve(entity.getComponent<TransformComponent>().position);
        }
    }
};

#endif