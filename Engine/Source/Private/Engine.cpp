#include <Engine.h>

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

Engine::Engine(GLFWwindow* window, RenderTarget* renderTarget)
{
	m_Window = window;

	m_RenderTarget = renderTarget;
}

Engine::~Engine()
{

}

bool Engine::Init()
{
	std::filesystem::current_path("Assets");

	//const GLFWwindow* window = world.get<GLFWwindow*>();

	glfwSetWindowUserPointer(m_Window, this);

	glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
	glfwSetCursorEnterCallback(m_Window, CursorEnterCallback);
	glfwSetCursorPosCallback(m_Window, MouseCallback);

	glfwSetWindowSizeLimits(m_Window, 304, 190, GLFW_DONT_CARE, GLFW_DONT_CARE);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return false;
	}

	std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glCheckError();

	flecs::entity stevieNicksCube = world.entity("StevieNicksCube");

	stevieNicksCube.add<TransformComponent>();
	stevieNicksCube.add<MeshComponent>();

	//flecs::entity camera = world.entity("MainCamera");

	flecs::system renderInitSystem = world.system<MeshComponent>()
		.each([](MeshComponent& meshComponent)
			{
				meshComponent.shader = Shader("CubeShader.vert", "CubeShader.frag");

				glGenVertexArrays(1, &meshComponent.VAO);
				glCheckError();
				glGenBuffers(1, &meshComponent.VBO);
				glCheckError();
				glBindVertexArray(meshComponent.VAO);
				glCheckError();
				glBindBuffer(GL_ARRAY_BUFFER, meshComponent.VBO);
				glCheckError();
				glBufferData(GL_ARRAY_BUFFER, sizeof(meshComponent.vertices), &meshComponent.vertices[0], GL_STATIC_DRAW);
				glCheckError();

				// Position Attribute
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
				glCheckError();
				glEnableVertexAttribArray(0);
				glCheckError();

				// Normals Attribute
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
				glCheckError();
				glEnableVertexAttribArray(1);
				glCheckError();

				// Texture Coord Attribute
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
				glCheckError();
				glEnableVertexAttribArray(2);
				glCheckError();

				// TEXTURE
				glGenTextures(1, &meshComponent.texture);
				glCheckError();
				glBindTexture(GL_TEXTURE_2D, meshComponent.texture);
				glCheckError();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glCheckError();
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glCheckError();
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glCheckError();
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glCheckError();

				int width, height, nrChannels;
				stbi_set_flip_vertically_on_load(true);
				unsigned char* data = stbi_load("stevie-nicks.jpg", &width, &height, &nrChannels, 0);

				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glCheckError();
					glGenerateMipmap(GL_TEXTURE_2D);
					glCheckError();
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}

				stbi_image_free(data);
			}
		);
	renderInitSystem.run();

	m_RenderTarget->Init();

	renderSystem = world.system<TransformComponent, MeshComponent>()
		.each([](TransformComponent& transformComponent, MeshComponent& meshComponent)
			{
				glActiveTexture(GL_TEXTURE0);
				glCheckError();
				glBindTexture(GL_TEXTURE_2D, meshComponent.texture);
				glBindTexture(GL_TEXTURE_2D, 1);
				glCheckError();

				meshComponent.shader.Use();

				meshComponent.shader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
				meshComponent.shader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
				meshComponent.shader.SetVec3("lightPos", glm::vec3(3.0f, 10.0f, -10.0f));

				meshComponent.shader.SetVec3("viewPos", glm::vec3(0.0f, 0.0f, -10.0f));

				//const float radius = 10.0f;
				//float camX = sin(glfwGetTime()) * radius;
				//float camZ = cos(glfwGetTime()) * radius;

				int width = 1280;
				int height = 720;
				//glfwGetWindowSize(m_Window, &width, &height);

				glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 1000.0f);
				meshComponent.shader.SetMat4("projection", projection);

				//glm::mat4 view = camera.GetViewMatrix();
				meshComponent.shader.SetMat4("view", glm::lookAt(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

				glBindVertexArray(meshComponent.VAO);
				glCheckError();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, transformComponent.Position);
				model *= glm::toMat4(transformComponent.Rotation);
				model = glm::scale(model, transformComponent.Scale);
				//float angle = 20.0f * (i + 1);
				//model = glm::rotate(model, (float)glfwGetTime() * 0.005f * (i + 10) * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				meshComponent.shader.SetMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
				glCheckError();
			}
		);

	renderShutdownSystem = world.system<MeshComponent>()
		.each([](MeshComponent& meshComponent)
			{
				glDeleteVertexArrays(1, &meshComponent.VAO);
				glCheckError();
				glDeleteBuffers(1, &meshComponent.VBO);
				glCheckError();
			}
		);

	return true;
}

void Engine::Terminate()
{
	renderShutdownSystem.run();

	glfwTerminate();
}

void Engine::MainLoop()
{
	lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(m_Window))
	{
		double currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		fps = 1.0f / deltaTime;

		//std::cout << "fps > " << fps << std::endl;

		ProcessInput(m_Window);

		m_RenderTarget->Bind();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glCheckError();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();

		renderSystem.run();

		m_RenderTarget->Unbind();

		m_RenderTarget->Render();

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}

void Engine::CursorEnterCallback(GLFWwindow* window, int entered)
{
	//if (entered)
	//{
		//firstMouse = true;

		//int width, height;
		//glfwGetWindowSize(window, &width, &height);
		//glfwSetCursorPos(window, width / 2.0f, height / 2.0f);
	//}
}

void Engine::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
	if (engine)
	{
		engine->HandleFramebuffer(width, height);
	}
}

void Engine::MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
	if (engine)
	{
		engine->HandleMouse(window, xPosIn, yPosIn);
	}
}

void Engine::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	//camera.ProcessMouseScroll((float)yOffset);
}

void Engine::HandleFramebuffer(int width, int height)
{
	m_RenderTarget->WindowResize(width, height);
}

void Engine::HandleMouse(GLFWwindow* window, double xPosIn, double yPosIn)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		float xPos = (float)xPosIn;
		float yPos = (float)yPosIn;

		if (!mouseDown)
		{
			lastX = xPos;
			lastY = yPos;
			mouseDown = true;
		}

		float xOffset = xPos - lastX;
		float yOffset = lastY - yPos;

		lastX = xPos;
		lastY = yPos;

		//camera.ProcessMouseMovement(window, xOffset, yOffset);
	}
	else if (mouseDown)
	{
		mouseDown = false;
	}
}

void Engine::HandleCursorEnter()
{

}

void Engine::HandleScroll()
{

}

void Engine::ProcessInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		//camera.ProcessInput(window, deltaTime);
	}
}