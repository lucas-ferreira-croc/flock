#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "base_light.hpp"

struct LightAttenuation
{
	float constant = 1.0f;
	float linear = 0.0f;
	float exp = 0.0f;
};

class PointLight : public BaseLight
{
public:
	glm::vec3 position = glm::vec3(0.0f);
	LightAttenuation attenuation;

};

#endif