#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <cgltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <flecs/flecs.h>

#include <Shader.h>

#include <TransformComponent.h>
#include <MeshComponent.h>
#include <TextureLoadComponent.h>
#include <SkyboxComponent.h>
#include <CubemapLoadComponent.h>
#include <CameraComponent.h>
#include <InputComponent.h>
#include <PlayerMovementComponent.h>
#include <PlayerYawComponent.h>
#include <PlayerPitchComponent.h>

#include <RenderSystem.h>

#include <RenderTarget.h>

#include <vector>
#include <filesystem>

#include <thread>
#include <future>

class Engine
{
public:
	GLFWwindow* m_Window;
	
	flecs::world m_World;

	flecs::entity mainCamera;

	float lastX = 0.0f;
	float lastY = 0.0f;
	bool mouseDown = false;

	double deltaTime = 0.0;
	double lastFrameTime = 0.0;
	unsigned int fps = 0;

	Engine(int width, int height, const char* name, RenderTarget* renderTarget);
	~Engine();

	bool Init();
	void InitSystems();
	void Terminate();
	void MainLoop();

private:
	flecs::system m_RenderShutdownSystem;

	flecs::query<const TransformComponent, const TransformComponent*, TransformComponent> m_GlobalTransformQuery;

	RenderTarget* m_RenderTarget;

	unsigned int framebuffer = 0;

	static TextureData LoadTexture(const char* path, bool flip);

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CursorEnterCallback(GLFWwindow* window, int entered);
	static void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

	void HandleFramebufferSize(int width, int height);
	void HandleKey(int key, int scancode, int action, int mods);
	void HandleCursorEnter();
	void HandleMouseMove(GLFWwindow* window, double xPos, double yPos);
	void HandleScroll();
};

void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);