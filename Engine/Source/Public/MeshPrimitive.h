#pragma once

#include <vector>

#include <Vertex.h>
#include <Texture.h>
#include <Material.h>

struct MeshPrimitive
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<Texture> textures;
	Material material;

	//unsigned int texture = 0;
	unsigned int VBO = 0;
	unsigned int VAO = 0;
	unsigned int EBO = 0;
};