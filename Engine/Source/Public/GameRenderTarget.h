#pragma once

#include <RenderTarget.h>

#include <Shader.h>

#include <iostream>

class GameRenderTarget : public RenderTarget
{
public:
	void Init() override;
	void Bind() override;
	void Unbind() override;
	void Render() override;

private:
	unsigned int framebuffer;
	unsigned int framebufferTexture;
	unsigned int screenVAO;
	Shader screenShader;
};