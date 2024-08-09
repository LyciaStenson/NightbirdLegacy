#include <Camera.h>

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
	updateCameraVectors();
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
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
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

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= Sensitivity;
	yOffset *= Sensitivity;

	Yaw += xOffset;
	Pitch += yOffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	updateCameraVectors();
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

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}
