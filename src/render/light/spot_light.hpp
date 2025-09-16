#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include "point_light.hpp"

class SpotLight : public PointLight
{
public:
	glm::vec3 direction = glm::vec3(0.0f);
	float cutoff = 0.0f;
    float outerCutofff = 0.0f;
};

#endif