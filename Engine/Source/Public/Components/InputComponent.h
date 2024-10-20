#pragma once

#include <glm/glm.hpp>

struct InputComponent
{
	bool moveForward = false;
	bool moveBackward = false;
	bool moveRight = false;
	bool moveLeft = false;

	bool moveUp = false;
	bool moveDown = false;

	glm::vec2 lookDir = glm::vec2(0.0f);
};