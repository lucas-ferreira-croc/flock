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
            entity.getComponent<FABRIKComponent>().solve();
        }
    }
};

class MultiEndedIKSystem : public System 
{
public:
    MultiEndedIKSystem()
    {
        requireComponent<MultiEndedFABRIKComponent>();
        requireComponent<TransformComponent>();
    }

    void Update(std::vector<glm::vec3>& entityPositions)
    {
        for(auto& entity : getSystemEntities())
        {   
            Logger::log("updting multi ended ik system");
            auto& multiEndedFabrik =  entity.getComponent<MultiEndedFABRIKComponent>();
            multiEndedFabrik.update(entityPositions);
        }
    }
};

#endif