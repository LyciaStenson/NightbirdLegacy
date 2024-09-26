#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

class RenderTarget
{
public:
	virtual ~RenderTarget() = default;
	virtual void Init() = 0;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void Render() = 0;

	virtual void GetWindowSize(int& aWidth, int& aHeight) = 0;

	virtual void WindowResize(int aWidth, int aHeight) = 0;

protected:
	int width = 0;
	int height = 0;

	unsigned int framebuffer = 0;
	unsigned int framebufferTexture = 0;
	unsigned int rbo = 0;
};