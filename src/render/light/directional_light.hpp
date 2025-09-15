#ifndef DIRECTIONAL_LIGHT_HPP
#define  DIRECTIONAL_LIGHT_HPP

#include "base_light.hpp"
#include <glm/glm.hpp>

class DirectionalLight : public BaseLight
{
public:
    DirectionalLight() 
        : BaseLight()
    {

    }

    DirectionalLight(glm::vec3 color, float ambientIntensity, float diffuseIntensity) 
        : BaseLight(color, ambientIntensity, diffuseIntensity)
    {

    }
	glm::vec3 direction;
};

#endif