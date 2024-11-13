#include <GameRenderTarget.h>

GameRenderTarget::GameRenderTarget(int aWidth, int aHeight)
{
	m_Width = aWidth;
	m_Height = aHeight;
}

GameRenderTarget::~GameRenderTarget()
{

}

void GameRenderTarget::Init(GLFWwindow* window)
{
	float screenVertices[] =
	{
		// Positions	// Texture Coords
		-1.0f,  1.0f,	0.0f, 1.0f, // Top left
		-1.0f, -1.0f,	0.0f, 0.0f, // Bottom left
		 1.0f, -1.0f,	1.0f, 0.0f, // Bottom right
		-1.0f,  1.0f,	0.0f, 1.0f, // Top left
		 1.0f, -1.0f,	1.0f, 0.0f, // Bottom right
		 1.0f,  1.0f,	1.0f, 1.0f	// Top right
	};

	unsigned int screenVBO;

	glGenVertexArrays(1, &m_ScreenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(m_ScreenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	m_ScreenShader.Load("ScreenShader.vert", "ScreenShader.frag");

	m_ScreenShader.Use();
	m_ScreenShader.SetInt("screenTexture", 0);

	glGenFramebuffers(1, &m_Framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	glGenTextures(1, &m_FramebufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

	glGenRenderbuffers(1, &m_Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer is not complete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameRenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	glEnable(GL_DEPTH_TEST);
}

void GameRenderTarget::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}

void GameRenderTarget::Render()
{
	m_ScreenShader.Use();
	glBindVertexArray(m_ScreenVAO);
	glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GameRenderTarget::GetWindowSize(int& width, int& height)
{
	width = m_Width;
	height = m_Height;
}

void GameRenderTarget::WindowResize(int width, int height)
{
	if (width <= 0 && height <= 0)
		return;

	m_Width = width;
	m_Height = height;

	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer is not complete" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, m_Width, m_Height);
}

bool GameRenderTarget::ShouldRun()
{
	return true;
}