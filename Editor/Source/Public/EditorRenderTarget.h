#pragma once

#include <RenderTarget.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

class EditorRenderTarget : public RenderTarget
{
public:
	EditorRenderTarget(GLFWwindow* aWindow);
	~EditorRenderTarget();
	void Init() override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

private:
	GLFWwindow* window;

	unsigned int framebuffer;
	unsigned int framebufferTexture;
};