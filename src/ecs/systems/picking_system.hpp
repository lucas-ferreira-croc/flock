#ifndef PICKING_SYSTEM_HPP
#define PICKING_SYSTEM_HPP

#include "ecs/ecs.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/id_component.hpp"
#include "log/logger.hpp"

class PickingSystem : public System
{
public:
    PickingSystem()
    {
        requireComponent<TransformComponent>();
    }

    void Update(const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
    {
         const float epsilon = 2.0f; // aumenta pra facilitar debug (ajusta depois)
        int i = 0;
        for(auto& entity : getSystemEntities())
        {
            auto& transform = entity.getComponent<TransformComponent>();
            glm::vec3 position = transform.position;

            glm::vec3 diff = position - rayOrigin;
            float distance = glm::length(glm::cross(diff, rayDirection)) / glm::length(rayDirection);

            if (distance < epsilon)
            {
                if (fabs(rayDirection.z) < 1e-6f) continue;

                float meshZ = position.z;
                float t = (meshZ - rayOrigin.z) / rayDirection.z;

                if (t <= 0.0f) continue; 

                glm::vec3 hitPoint = rayOrigin + t * rayDirection;
                //transform.position = glm::vec3(hitPoint.x, hitPoint.y, meshZ);

                auto id = entity.getComponent<IDComponent>()._name;
            }
        }
    }
};

#endif