#pragma once

#include <glm/glm.hpp>

#include <Shader.h>

struct OceanComponent
{
	float vertices[12] =
	{
		-25.0f,  0.0f, -25.0f,
		-25.0f,  0.0f,  25.0f,
		 25.0f,  0.0f, -25.0f,
		 25.0f,  0.0f,  25.0f
	};

	unsigned int VBO;
	unsigned int VAO;
	
	glm::vec4 color;
	float radius;
	float waveAmplitude;
	float waveFrequency;
	
	Shader shader;
};