#include "base_light.hpp"

BaseLight::BaseLight(glm::vec3 color, float ambientIntensity, float diffuseIntensity, float specularIntensity)
    : color(color), ambientIntensity(ambientIntensity), diffuseIntensity(diffuseIntensity), specularIntensity(specularIntensity)
{

}

BaseLight::BaseLight()
	: color(1.0f, 1.0f, 1.0f), ambientIntensity(0.0f)
{
}

BaseLight::BaseLight(float r, float g, float b, float intensity)
	: color(r, g, b), ambientIntensity(intensity)
{
}

BaseLight::BaseLight(glm::vec3 color, float intensity)
	: color(color), ambientIntensity(intensity)
{
}

BaseLight::~BaseLight()
{
}
