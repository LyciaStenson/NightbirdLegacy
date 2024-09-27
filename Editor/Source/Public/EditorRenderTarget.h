#pragma once

#include <Engine.h>

#include <RenderTarget.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <string>
#include <vector>

#include <flecs/flecs.h>

class EditorRenderTarget : public RenderTarget
{
public:
	EditorRenderTarget(GLFWwindow* aWindow);
	~EditorRenderTarget();
	void Init() override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

	void GetWindowSize(int& aWidth, int& aHeight) override;

	void WindowResize(int aWidth, int aHeight) override;

	flecs::world* world = nullptr;

private:
	int sceneWidth = 1280;
	int sceneHeight = 720;

	Engine* engine;

	std::vector<std::string> consoleText;

	bool showAboutWindow = false;
	bool showEntitiesWindow = true;
	bool showComponentsWindow = true;
	bool showAssetBrowserWindow = true;
	bool showConsoleWindow = true;
	bool showSceneWindow = true;

	GLFWwindow* window;

	void Log(const std::string& text);
};