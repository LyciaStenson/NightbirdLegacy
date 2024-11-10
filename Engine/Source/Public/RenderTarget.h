#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

class RenderTarget
{
public:
	virtual ~RenderTarget() = default;
	virtual void Init(GLFWwindow* window) = 0;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void Render() = 0;

	virtual void GetWindowSize(int& width, int& height) = 0;

	virtual void WindowResize(int width, int height) = 0;

	virtual bool ShouldRun() = 0;

protected:
	int m_Width = 0;
	int m_Height = 0;

	unsigned int m_Framebuffer = 0;
	unsigned int m_FramebufferTexture = 0;
	unsigned int m_Rbo = 0;
};