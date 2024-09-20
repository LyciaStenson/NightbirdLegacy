#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct TransformComponent
{
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;
};