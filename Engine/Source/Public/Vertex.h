#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 baseColorTexCoords;
	glm::vec2 metallicRoughnessTexCoords;

	Vertex()
		: position(glm::vec3()), normal(glm::vec3()), baseColorTexCoords(glm::vec2())
	{
	}

	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords)
		: position(position), normal(normal), baseColorTexCoords(texCoords)
	{
	}
};