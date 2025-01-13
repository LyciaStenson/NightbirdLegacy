#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 baseColorTexCoords;
	glm::vec2 metallicRoughnessTexCoords;
	glm::vec2 normalTexCoords;
	
	Vertex()
		: position(glm::vec3()), normal(glm::vec3()), baseColorTexCoords(glm::vec2()), metallicRoughnessTexCoords(glm::vec2()), normalTexCoords(glm::vec2())
	{
	}

	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 baseColorTexCoords = glm::vec2(), glm::vec2 metallicRoughnessTexCoords = glm::vec2(), glm::vec2 normalTexCoords = glm::vec2())
		: position(position), normal(normal), baseColorTexCoords(baseColorTexCoords), metallicRoughnessTexCoords(metallicRoughnessTexCoords), normalTexCoords(normalTexCoords)
	{
	}
};