#pragma once

#include <glm/glm.hpp>

struct CameraComponent
{
	float Yaw;
	float Pitch;

	float MovementSpeed;
	float Sensitivity;
	float Fov;
};