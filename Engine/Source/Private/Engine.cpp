#include <Engine.h>

Engine::Engine(GLFWwindow* aWindow, RenderTarget* aRenderTarget)
{
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	Window = aWindow;

	renderTarget = aRenderTarget;
}

Engine::~Engine()
{

}

bool Engine::Init()
{
	try
	{
		std::filesystem::current_path("Assets");
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to set working directory to Assets: " << e.what() << '\n';
	}

	glfwSetWindowUserPointer(Window, this);

	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
	glfwSetCursorEnterCallback(Window, CursorEnterCallback);
	glfwSetCursorPosCallback(Window, MouseCallback);

	glfwSetWindowSizeLimits(Window, 304, 190, GLFW_DONT_CARE, GLFW_DONT_CARE);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return false;
	}

	std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

	glEnable(GL_DEPTH_TEST);

	CubeShader = NBShader("CubeShader.vert", "CubeShader.frag");

	//TransformComponent transformComponent1 = TransformComponent();
	//transformComponent1.Position = glm::vec3(56.0f, 23.0f, 90.0f);
	//Scene.AddComponent<TransformComponent>(0, transformComponent1);

	//const TransformComponent& transformComponent2 = Scene.GetComponent<TransformComponent>(0);
	//std::cout << "transformComponent2.Position > " << transformComponent2.Position.x << ", " << transformComponent2.Position.y << ", " << transformComponent2.Position.z << std::endl;

	// Init TransformComponents

	for (int i = 0; i < 500; i++)
	{
		Scene.AddComponent(i, TransformComponent());
		Scene.GetComponent<TransformComponent>(i).Position = glm::vec3(i, 1.0f, 0.0f);
	}

	//Scene.AddComponent(0, TransformComponent());
	//Scene.GetComponent<TransformComponent>(0).Position = glm::vec3(0.0f, 0.0f, 0.0f);

	//Scene.AddComponent(1, TransformComponent());
	//Scene.GetComponent<TransformComponent>(1).Position = glm::vec3(2.0f, 5.0f, -15.0f);

	//Scene.AddComponent(2, TransformComponent());
	//Scene.GetComponent<TransformComponent>(2).Position = glm::vec3(-1.5f, -2.2f, -2.5f);

	//Scene.AddComponent(3, TransformComponent());
	//Scene.GetComponent<TransformComponent>(3).Position = glm::vec3(-3.8f, -2.0f, -12.3f);

	//Scene.AddComponent(4, TransformComponent());
	//Scene.GetComponent<TransformComponent>(4).Position = glm::vec3(2.4f, -0.4f, -3.5f);

	//Scene.AddComponent(5, TransformComponent());
	//Scene.GetComponent<TransformComponent>(5).Position = glm::vec3(-1.7f, 3.0f, -7.5f);

	//Scene.AddComponent(6, TransformComponent());
	//Scene.GetComponent<TransformComponent>(6).Position = glm::vec3(1.3f, -2.0f, -2.5f);

	//Scene.AddComponent(7, TransformComponent());
	//Scene.GetComponent<TransformComponent>(7).Position = glm::vec3(1.5f, 2.0f, -2.5f);

	//Scene.AddComponent(8, TransformComponent());
	//Scene.GetComponent<TransformComponent>(8).Position = glm::vec3(1.5f, 0.2f, -1.5f);

	//Scene.AddComponent(9, TransformComponent());
	//Scene.GetComponent<TransformComponent>(9).Position = glm::vec3(-1.3f, 1.0f, -1.5f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices[0], GL_STATIC_DRAW);

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
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("stevie-nicks.jpg", &width, &height, &nrChannels, 0);

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

	renderTarget->Init();

	return true;
}

void Engine::Terminate()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
}

void Engine::MainLoop()
{
	lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(Window))
	{
		double currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		fps = 1.0f / deltaTime;

		//std::cout << "fps > " << fps << std::endl;

		ProcessInput(Window);

		renderTarget->Bind();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		CubeShader.Use();

		CubeShader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		CubeShader.SetVec3("lightPos", glm::vec3(3.0f, 10.0f, 10.0f));

		CubeShader.SetVec3("viewPos", camera.Position);

		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		int width, height;
		glfwGetWindowSize(Window, &width, &height);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)width / (float)height, 0.1f, 1000.0f);
		CubeShader.SetMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		CubeShader.SetMat4("view", view);

		glBindVertexArray(VAO);


		for (unsigned int i = 0; i < 500; i++)
		{
			const TransformComponent& transformComponent = Scene.GetComponent<TransformComponent>(i);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, transformComponent.Position);
			model *= glm::toMat4(transformComponent.Rotation);
			model = glm::scale(model, transformComponent.Scale);
			float angle = 20.0f * (i + 1);
			//model = glm::rotate(model, (float)glfwGetTime() * 0.005f * (i + 10) * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			CubeShader.SetMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		renderTarget->Unbind();
		
		renderTarget->Render();

		glfwSwapBuffers(Window);
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
		
		camera.ProcessMouseMovement(window, xOffset, yOffset);
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
		camera.ProcessInput(window, deltaTime);
	}
}