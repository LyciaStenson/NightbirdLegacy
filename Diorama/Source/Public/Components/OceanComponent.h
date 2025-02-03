#pragma once

#include <glm/glm.hpp>

#include <Shader.h>

struct OceanComponent
{
	float vertices[12] =
	{
		-50.0f,  0.0f, -50.0f,
		-50.0f,  0.0f,  50.0f,
		 50.0f,  0.0f, -50.0f,
		 50.0f,  0.0f,  50.0f
	};

	unsigned int VBO;
	unsigned int VAO;
	
	glm::vec3 color;
	float radius;
	
	Shader shader;
};