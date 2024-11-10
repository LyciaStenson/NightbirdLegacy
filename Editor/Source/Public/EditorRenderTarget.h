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
	EditorRenderTarget();
	~EditorRenderTarget();
	void Init(GLFWwindow* window) override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

	void GetWindowSize(int& width, int& height) override;

	void WindowResize(int width, int height) override;

	bool ShouldRun() override;

	flecs::world m_EditorWorld;

private:
	bool m_shouldRun = false;

	int m_SceneWidth = 1280;
	int m_SceneHeight = 720;

	std::vector<std::string> m_ConsoleText;

	bool m_ShowAboutWindow = false;
	bool m_ShowEntitiesWindow = true;
	bool m_ShowComponentsWindow = true;
	bool m_ShowAssetBrowserWindow = true;
	bool m_ShowConsoleWindow = true;
	bool m_ShowSceneWindow = true;
	
	GLFWwindow* m_Window;

	void Log(const std::string& text);
};