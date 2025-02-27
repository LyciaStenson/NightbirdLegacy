#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include <flecs/flecs.h>

#include <ResourceManager.h>

#include <Shader.h>

#include <TransformComponent.h>
#include <MeshComponent.h>
#include <TextureLoadComponent.h>
#include <SkyboxComponent.h>
#include <CubemapLoadComponent.h>
#include <LightComponent.h>
#include <CameraComponent.h>
#include <InputComponent.h>
#include <PlayerMovementComponent.h>
#include <PlayerYawComponent.h>
#include <PlayerPitchComponent.h>

#include <RenderTarget.h>

#include <vector>
#include <filesystem>

#include <functional>

#include <thread>
#include <future>

class Engine
{
public:
	using KeyCallbackFunc = std::function<void(Engine* engine, int key, int scancode, int action, int mods)>;
	using MouseButtonCallbackFunc = std::function<void(Engine* engine, int button, int action, int mods)>;

	GLFWwindow* m_Window;
	
	flecs::world m_World;

	flecs::entity m_MainCamera;

	flecs::entity m_DirectionalLight;

	float lastX = 0.0f;
	float lastY = 0.0f;
	bool mouseDown = false;

	double deltaTime = 0.0;
	double lastFrameTime = 0.0;
	unsigned int fps = 0;

	bool globalNormalMapsEnabled = true;
	bool globalShadowMapsEnabled = true;

	Engine(int width, int height, const char* name, RenderTarget* renderTarget);
	~Engine();
	
	bool Init();
	void InitSystems();
	void Terminate();
	void MainLoop();

	void RegisterKeyCallback(KeyCallbackFunc callback);
	void RegisterMouseButtonCallback(MouseButtonCallbackFunc callback);

	ResourceManager& GetResourceManager();

private:
	ResourceManager m_ResourceManager;

	flecs::system m_RenderShutdownSystem;

	flecs::query<const TransformComponent, const TransformComponent*, TransformComponent> m_GlobalTransformQuery;
	
	flecs::query<BaseLightComponent, PointLightComponent, flecs::pair<TransformComponent, Global>> m_PointLightQuery;

	RenderTarget* m_RenderTarget;

	std::vector<KeyCallbackFunc> m_keyCallbacks;
	std::vector<MouseButtonCallbackFunc> m_mouseButtonCallbacks;
	
	static TextureData LoadTexture(const char* path, bool flipVertically);
	
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
	static void CursorEnterCallback(GLFWwindow* window, int entered);
	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

	void HandleFramebufferSize(int width, int height);
	void HandleKey(int key, int scancode, int action, int mods);
	void HandleMouseButton(int button, int action, int mods);
	void HandleMouseMove(GLFWwindow* window, double xPos, double yPos);
	void HandleCursorEnter();
	void HandleScroll();
};

void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);