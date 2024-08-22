#pragma once

#include <Shader.h>

#include <vector>

class StevieNicksCube
{
public:
	StevieNicksCube(glm::vec3 aPosition);
	~StevieNicksCube();

	void Update(float deltaTime);

	glm::vec3 position;
	
	std::vector<float> vertices;

private:

	Shader cubeShader;
};