#pragma once

#include <RenderTarget.h>

#include <Shader.h>

#include <iostream>

class GameRenderTarget : public RenderTarget
{
public:
	GameRenderTarget(int aWidth, int aHeight);
	~GameRenderTarget();

	void Init(GLFWwindow* window) override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

	void GetWindowSize(int& width, int& height) override;

	void WindowResize(int width, int height) override;

	bool ShouldRun() override;

private:
	unsigned int m_Framebuffer = 0;
	unsigned int m_FramebufferTexture = 0;
	unsigned int m_ScreenVAO = 0;
	Shader m_ScreenShader;
};