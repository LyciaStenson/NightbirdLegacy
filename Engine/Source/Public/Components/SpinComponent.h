#pragma once

#include <glm/glm.hpp>

struct SpinComponent
{
	float speed = 0.0f;

	glm::quat rotation = glm::quat();
};