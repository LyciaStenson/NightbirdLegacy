#pragma once

#include <glm/glm.hpp>

#include <Shader.h>

struct OceanComponent
{
	float vertices[12] =
	{
		-100.0f,  0.0f, -100.0f,
		-100.0f,  0.0f,  100.0f,
		 100.0f,  0.0f, -100.0f,
		 100.0f,  0.0f,  100.0f
	};

	unsigned int VBO;
	unsigned int VAO;

	glm::vec3 color;

	Shader shader;
};