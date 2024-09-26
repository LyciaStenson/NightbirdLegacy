#include <GameRenderTarget.h>

GameRenderTarget::GameRenderTarget(int aWidth, int aHeight)
{
	width = aWidth;
	height = aHeight;
}

GameRenderTarget::~GameRenderTarget()
{

}

void GameRenderTarget::Init()
{
	float screenVertices[] =
	{
		// Positions	// Texture Coords
		-1.0f,  1.0f,	0.0f, 0.0f, // Top left
		-1.0f, -1.0f,	0.0f, 1.0f, // Bottom left
		 1.0f, -1.0f,	1.0f, 1.0f, // Bottom right
		-1.0f,  1.0f,	0.0f, 0.0f, // Top left
		 1.0f, -1.0f,	1.0f, 1.0f, // Bottom right
		 1.0f,  1.0f,	1.0f, 0.0f	// Top right
	};

	unsigned int screenVBO;

	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	screenShader.Load("ScreenShader.vert", "ScreenShader.frag");

	screenShader.Use();
	screenShader.SetInt("screenTexture", 0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer is not complete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameRenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
}

void GameRenderTarget::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}

void GameRenderTarget::Render()
{
	screenShader.Use();
	glBindVertexArray(screenVAO);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GameRenderTarget::GetWindowSize(int& aWidth, int& aHeight)
{
	aWidth = width;
	aHeight = height;
}

void GameRenderTarget::WindowResize(int aWidth, int aHeight)
{
	width = aWidth;
	height = aHeight;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer is not complete" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, width, height);
}