#pragma once

struct InputComponent
{
	bool moveForward = false;
	bool moveBackward = false;
	bool moveRight = false;
	bool moveLeft = false;

	bool moveUp = false;
	bool moveDown = false;

	float lookX = 0.0f;
	float lookY = 0.0f;
};