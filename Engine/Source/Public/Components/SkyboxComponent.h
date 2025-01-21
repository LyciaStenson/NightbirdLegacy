#pragma once

#include <vector>

struct SkyboxComponent
{
    float vertices[108] =
	{
		// Positions
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f, 
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	unsigned int indices[36] =
	{
		// Right
		1, 5, 6,
		6, 2, 1,
		// Left
		0, 3, 7,
		7, 4, 0,
		// Top
		4, 7, 6,
		6, 5, 4,
		// Bottom
		0, 1, 2,
		2, 3, 0,
		// Back
		0, 4, 5,
		5, 1, 0,
		// Front
		3, 2, 6,
		6, 7, 3
	};
	
	unsigned int texture = 0;
	unsigned int VBO = 0;
	unsigned int VAO = 0;
	unsigned int EBO = 0;
	
	const char* vertexPath;
	const char* fragmentPath;
	std::vector<const char*> texturePaths;

	Shader shader;
};