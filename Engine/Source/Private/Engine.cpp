#include <Engine.h>

Engine::Engine(GLFWwindow* aWindow, RenderTarget* aRenderTarget)
{
	m_Window = aWindow;

	renderTarget = aRenderTarget;
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
	
	flecs::entity e = world.entity();

	e.add<TransformComponent>();
	e.add<MeshComponent>();

	auto camera = world.entity("MainCamera");

	flecs::system shaderInitSystem = world.system<MeshComponent>()
		.each([](MeshComponent meshComponent)
			{
				meshComponent.shader = Shader("CubeShader.vert", "CubeShader.frag");
			}
		);
	shaderInitSystem.run();

	flecs::system transformInitSystem = world.system<TransformComponent>()
		.each([](TransformComponent transformComponent)
			{
				transformComponent.Position = glm::vec3(0.0f, 0.0f, 10.0f);
			}
		);
	transformInitSystem.run();

	flecs::system renderInitSystem = world.system<MeshComponent>()
		.each([](MeshComponent meshComponent)
			{
				glGenVertexArrays(1, &meshComponent.VAO);
				glGenBuffers(1, &meshComponent.VBO);
				glBindVertexArray(meshComponent.VAO);
				glBindBuffer(GL_ARRAY_BUFFER, meshComponent.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(meshComponent.vertices), &meshComponent.vertices[0], GL_STATIC_DRAW);

				// Position Attribute
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
				glEnableVertexAttribArray(0);

				// Normals Attribute
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);

				// Texture Coord Attribute
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
				glEnableVertexAttribArray(2);

				// TEXTURE
				glGenTextures(1, &meshComponent.texture);
				glBindTexture(GL_TEXTURE_2D, meshComponent.texture);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				int width, height, nrChannels;
				stbi_set_flip_vertically_on_load(true);
				unsigned char* data = stbi_load("stevie-nicks.jpg", &width, &height, &nrChannels, 0);

				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}

				stbi_image_free(data);
			}
		);
	renderInitSystem.run();

	renderTarget->Init();

	renderSystem = world.system<TransformComponent, MeshComponent>()
		.each([](TransformComponent transformComponent, MeshComponent meshComponent)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, meshComponent.texture);

				meshComponent.shader.Use();

				//meshComponent.shader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
				//meshComponent.shader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
				//meshComponent.shader.SetVec3("lightPos", glm::vec3(3.0f, 10.0f, 10.0f));

				//meshComponent.shader.SetVec3("viewPos", glm::vec3(0.0f, 0.0f, -10.0f));

				//const float radius = 10.0f;
				//float camX = sin(glfwGetTime()) * radius;
				//float camZ = cos(glfwGetTime()) * radius;
				
				int width = 1280;
				int height = 720;
				//glfwGetWindowSize(m_Window, &width, &height);

				glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 1000.0f);
				meshComponent.shader.SetMat4("projection", projection);

				//glm::mat4 view = camera.GetViewMatrix();
				meshComponent.shader.SetMat4("view", glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

				glBindVertexArray(meshComponent.VAO);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, transformComponent.Position);
				model *= glm::toMat4(transformComponent.Rotation);
				model = glm::scale(model, transformComponent.Scale);
				//float angle = 20.0f * (i + 1);
				//model = glm::rotate(model, (float)glfwGetTime() * 0.005f * (i + 10) * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				meshComponent.shader.SetMat4("model", model);
				
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		);

	renderShutdownSystem = world.system<MeshComponent>()
		.each([](MeshComponent meshComponent)
			{
				glDeleteVertexArrays(1, &meshComponent.VAO);
				glDeleteBuffers(1, &meshComponent.VBO);
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

		renderTarget->Bind();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderSystem.run();
		
		renderTarget->Unbind();
		
		renderTarget->Render();

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
	renderTarget->WindowResize(width, height);
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