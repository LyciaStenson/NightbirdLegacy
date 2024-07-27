#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float yaw = -90.0f;
const float pitch = 0.0f;
const float speed = 2.5f;
const float sensitivity = 0.1f;
const float zoom = 45.0f;