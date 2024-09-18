#pragma once

/*
#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Movement_Direction
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float MOVEMENT_SPEED = 3.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera
{
public:
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

	Camera(
		glm::vec3 positon = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH
	);

	Camera(
		float posX,
		float posY,
		float posZ,
		float upX,
		float upY,
		float upZ,
		float yaw,
		float pitch
	);

	glm::mat4 GetViewMatrix();

	void ProcessInput(GLFWwindow* window, float deltaTime);

	void ProcessMouseMovement(GLFWwindow* window, float xOffset, float yOffset, bool constrainPitch = true);

private:
	void UpdateCameraVectors();

	void ProcessKeyboard(Movement_Direction direction, float deltaTime);

	void ProcessMouseScroll(float yOffset);
};
*/