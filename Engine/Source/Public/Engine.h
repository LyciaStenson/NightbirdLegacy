#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>

#include <Camera.h>

#include <RenderTarget.h>

#include <filesystem>

class Engine
{
public:
	Camera camera;

	float lastX;
	float lastY;
	bool firstMouse;

	float deltaTime;
	float lastFrame;

	Engine(GLFWwindow *aWindow, RenderTarget *aRenderTarget);
	~Engine();

	bool Init();
	void Terminate();
	//void SetWindow(GLFWwindow* aWindow);
	//void CreateWindow(int width, int height, const char* title);
	void MainLoop();

private:
	GLFWwindow* window;

	RenderTarget* renderTarget;

	unsigned int texture;

	Shader cubeShader;

	unsigned int VBO, VAO, framebuffer;
	
	glm::vec3 cubePositions[10];

	static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
	static void cursorEnterCallback(GLFWwindow *window, int entered);
	static void mouseCallback(GLFWwindow *window, double xPos, double yPos);
	static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

	void handleFramebuffer();
	void handleWindow_focus();
	void handleCursor_enter();
	void handleMouse();
	void handleScroll();

	void processInput(GLFWwindow *window);
};