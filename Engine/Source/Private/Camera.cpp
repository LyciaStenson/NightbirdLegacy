#include <Camera.h>

#include <iostream>

Camera::Camera(
	glm::vec3 positon,
	glm::vec3 worldUp,
	float yaw,
	float pitch
) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	MovementSpeed(MOVEMENT_SPEED),
	Sensitivity(SENSITIVITY),
	Fov(FOV)
{
	Position = positon;
	WorldUp = worldUp;
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

Camera::Camera(
	float posX,
	float posY,
	float posZ,
	float upX,
	float upY,
	float upZ,
	float yaw,
	float pitch
) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	MovementSpeed(MOVEMENT_SPEED),
	Sensitivity(SENSITIVITY),
	Fov(FOV)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessInput(GLFWwindow* window, float deltaTime)
{
	float cameraSpeed = (float)(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		ProcessKeyboard(UP, deltaTime);
}

void Camera::ProcessKeyboard(Movement_Direction direction, float deltaTime)
{
	float speed = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * speed;
	if (direction == BACKWARD)
		Position -= Front * speed;
	if (direction == LEFT)
		Position -= Right * speed;
	if (direction == RIGHT)
		Position += Right * speed;
	if (direction == UP)
		Position += Up * speed;
	if (direction == DOWN)
		Position -= Up * speed;
}

void Camera::ProcessMouseMovement(GLFWwindow* window, float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= Sensitivity;
	yOffset *= Sensitivity;

	Yaw += xOffset;
	Pitch -= yOffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
	Fov -= (float)yOffset;
	if (Fov < 10.0f)
		Fov = 10.0f;
	if (Fov > 90.0f)
		Fov = 90.0f;
}

//private

void Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}
