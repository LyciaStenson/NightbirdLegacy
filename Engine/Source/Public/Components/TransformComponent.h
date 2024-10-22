#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct TransformComponent
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::quat Rotation = glm::quat();
	glm::vec3 Scale = glm::vec3(1.0f);
};

struct Local
{
};

struct Global
{
};