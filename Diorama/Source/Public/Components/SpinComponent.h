#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct SpinComponent
{
	float speed;

	glm::vec3 axis;
};