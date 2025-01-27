#pragma once

#include <glm/glm.hpp>

struct BaseLightComponent
{
	float intensity;
	glm::vec3 color;

	bool shadowMappingEnabled;
	int shadowTextureWidth, shadowTextureHeight;
	unsigned int screenShadowVAO;
	unsigned int shadowFramebuffer;
	unsigned int shadowTexture;

	Shader shadowScreenShader;
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