#pragma once

struct NeuronsComponent
{
	float vertices[8] = {
	-1.0f, -1.0f, // Bottom left
	 1.0f, -1.0f, // Bottom right
	 1.0f,  1.0f, // Top right
	-1.0f,  1.0f  // Top left
	};
	
	//float vertices[24] =
	//{
	//	// Positions	// Texture Coords
	//	-1.0f,  1.0f,	0.0f, 0.0f, // Top left
	//	-1.0f, -1.0f,	0.0f, 1.0f, // Bottom left
	//	 1.0f, -1.0f,	1.0f, 1.0f, // Bottom right
	//	-1.0f,  1.0f,	0.0f, 0.0f, // Top left
	//	 1.0f, -1.0f,	1.0f, 1.0f, // Bottom right
	//	 1.0f,  1.0f,	1.0f, 0.0f	// Top right
	//};
	
	unsigned int indices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	unsigned int VBO = 0;
	unsigned int VAO = 0;
	unsigned int EBO = 0;

	const char* vertexPath;
	const char* fragmentPath;

	Shader shader;
};