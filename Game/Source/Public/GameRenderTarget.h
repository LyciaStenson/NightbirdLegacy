#pragma once

#include <RenderTarget.h>

#include <Shader.h>

#include <iostream>

class GameRenderTarget : public RenderTarget
{
public:
	GameRenderTarget(int aWidth, int aHeight);
	~GameRenderTarget();

	void Init() override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

	void GetWindowSize(int aWidth, int aHeight) override;

	void WindowResize(int aWidth, int aHeight) override;

private:
	unsigned int framebuffer = 0;
	unsigned int framebufferTexture = 0;
	unsigned int screenVAO = 0;
	Shader screenShader;
};