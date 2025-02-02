#pragma once

#include <Shader.h>

struct OceanComponent
{
	float vertices[12] =
	{
		-1.0f, 0.0f, -1.0f,
		 1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f,  1.0f,
		 1.0f, 0.0f,  1.0f
	};

	unsigned int VBO;
	unsigned int VAO;

	Shader shader;
};