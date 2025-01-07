#pragma once

#include <vector>

#include <Vertex.h>
#include <Texture.h>
#include <Shader.h>

struct MeshComponent
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int texture = 0;
	unsigned int VBO = 0;
	unsigned int VAO = 0;
	unsigned int EBO = 0;

	const char* vertexPath;
	const char* fragmentPath;
	const char* texturePath;

	Shader shader;
};