#pragma once

#include <glm/glm.hpp>

#include <Shader.h>

struct Material
{
	glm::vec4 baseColor = glm::vec4(1.0f);
	float metallic = 0.0f;
	float roughness = 0.0f;
	float normalScale = 1.0f;

	unsigned int baseColorTexture = 0;
	unsigned int metallicRoughnessTexture = 0;
	unsigned int normalTexture = 0;

	bool hasBaseColorTexture = false;
	bool hasMetallicRoughnessTexture = false;
	bool hasNormalTexture = false;
	
	Shader shader;
	const char* vertexPath;
	const char* fragmentPath;
};