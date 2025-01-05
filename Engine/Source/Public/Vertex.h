#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords)
	{
		this->position = position;
		this->normal = normal;
		this->texCoords = texCoords;
	}
};