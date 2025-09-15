#ifndef BASE_LIGHT_HPP
#define BASE_LIGHT_HPP

#include "glm/glm.hpp"

struct BaseLight
{
    BaseLight();
    BaseLight(float r, float  g, float  b, float intensity);
    BaseLight(glm::vec3 color, float intensity);
    BaseLight(glm::vec3 color, float ambientIntensity, float diffuseIntensity);
	~BaseLight();

    glm::vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;

};

#endif