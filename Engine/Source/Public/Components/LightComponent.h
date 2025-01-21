#pragma once

#include <glm/glm.hpp>

struct BaseLightComponent
{
	float intensity;
	glm::vec3 color;
};

struct DirectionalLightComponent
{
	float ambient;
};

struct PointLightComponent
{
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	
	// This value will be used for optimisation
	float range;
};