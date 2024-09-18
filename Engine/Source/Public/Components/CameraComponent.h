#pragma once

#include <glm/glm.hpp>

struct CameraComponent
{
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float Sensitivity;
	float Fov;
};