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

	virtual void SetWindowSize(int aWidth, int aHeight);

protected:
	int width;
	int height;
};