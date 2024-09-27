#include <Engine.h>

struct Window
{
	GLFWwindow* window;
};

struct RenderTargetStruct
{
	RenderTarget* renderTarget;
};

struct Camera
{
	const TransformComponent* transformComponent;
	const CameraComponent* cameraComponent;
};

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

	glfwSetWindowUserPointer(m_Window, this);

	glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
	glfwSetCursorEnterCallback(m_Window, CursorEnterCallback);
	glfwSetCursorPosCallback(m_Window, MouseCallback);

	glfwSetWindowSizeLimits(m_Window, 304, 190, GLFW_DONT_CARE, GLFW_DONT_CARE);

	Window window({ m_Window });

	RenderTargetStruct renderTarget({ m_RenderTarget });

	m_World.set<Window>(window);
	m_World.set<RenderTargetStruct>(renderTarget);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return false;
	}

	std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glCheckError();

	flecs::entity parentTest = m_World.entity("ParentTest");
	parentTest.add<TransformComponent>();
	parentTest.add<SpinComponent>();

	parentTest.set<TransformComponent>({ glm::vec3(), glm::quat(), glm::vec3(1.0f) });

	flecs::entity stevieNicksCube = m_World.entity("StevieNicksCube")
		.child_of(parentTest);
	stevieNicksCube.add<TransformComponent>();
	stevieNicksCube.add<MeshComponent>();
	stevieNicksCube.add<SpinComponent>();

	stevieNicksCube.set<TransformComponent>({ glm::vec3(-1.0f, 0.0f, 0.0f), glm::quat(), glm::vec3(1.0f) });

	flecs::entity stevieNicksCube2 = m_World.entity("StevieNicksCube2")
		.child_of(parentTest);
	stevieNicksCube2.add<TransformComponent>();
	stevieNicksCube2.add<MeshComponent>();
	stevieNicksCube2.add<SpinComponent>();

	stevieNicksCube2.set<TransformComponent>({ glm::vec3(1.0f, 0.0f, 0.0f), glm::quat(), glm::vec3(1.0f) });

	flecs::entity camera = m_World.entity("Camera");
	camera.add<TransformComponent>();
	camera.add<CameraComponent>();

	Camera cameraStruct({ camera.get<TransformComponent>(), camera.get<CameraComponent>() });
	m_World.set<Camera>(cameraStruct);

	flecs::system renderInitSystem = m_World.system<MeshComponent>("RenderInitSystem")
		.kind(flecs::OnSet)
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

	m_RenderSystem = m_World.system<TransformComponent, MeshComponent>("RenderSystem")
		.kind(flecs::OnUpdate)
		.run([](flecs::iter& iter)
			{
				while (iter.next())
				{
					flecs::field<TransformComponent> transformComponent = iter.field<TransformComponent>(0);
					flecs::field<MeshComponent> meshComponent = iter.field<MeshComponent>(1);

					for (auto i : iter)
					{
						glActiveTexture(GL_TEXTURE0);
						glCheckError();
						glBindTexture(GL_TEXTURE_2D, meshComponent[i].texture);
						glBindTexture(GL_TEXTURE_2D, 1);
						glCheckError();

						meshComponent[i].shader.Use();

						meshComponent[i].shader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
						meshComponent[i].shader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
						meshComponent[i].shader.SetVec3("lightPos", glm::vec3(3.0f, 10.0f, -10.0f));

						meshComponent[i].shader.SetVec3("viewPos", glm::vec3(0.0f, 0.0f, -10.0f));

						flecs::world world = iter.world();
						const Window* window = world.get<Window>();

						const RenderTargetStruct* renderTarget = world.get<RenderTargetStruct>();

						int width;
						int height;
						renderTarget->renderTarget->GetWindowSize(width, height);

						glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 1000.0f);
						meshComponent[i].shader.SetMat4("projection", projection);

						const Camera* camera = world.get<Camera>();

						glm::mat4 view = glm::lookAt(camera->transformComponent->Position, camera->transformComponent->Position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));;
						meshComponent[i].shader.SetMat4("view", glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

						glBindVertexArray(meshComponent[i].VAO);
						glCheckError();

						TransformComponent globalTransform = transformComponent[i];

						flecs::entity current = iter.entity(i);
						while (flecs::entity parent = current.parent())
						{
							const TransformComponent* parentTransform = parent.get<TransformComponent>();
							if (parentTransform)
							{
								globalTransform.Scale *= parentTransform->Scale;
								globalTransform.Rotation *= parentTransform->Rotation;
								globalTransform.Position = parentTransform->Position + (parentTransform->Rotation * (parentTransform->Scale * globalTransform.Position));
							}
							current = parent;
						}

						glm::mat4 model = glm::mat4(1.0f);
						model = glm::translate(model, globalTransform.Position);
						model *= glm::toMat4(globalTransform.Rotation);
						model = glm::scale(model, globalTransform.Scale);
						meshComponent[i].shader.SetMat4("model", model);

						glDrawArrays(GL_TRIANGLES, 0, 36);
						glCheckError();
					}
				}
			}
		);

	m_RenderShutdownSystem = m_World.system<MeshComponent>("RenderShutdownSystem")
		.kind(flecs::OnSet)
		.each([](MeshComponent& meshComponent)
			{
				glDeleteVertexArrays(1, &meshComponent.VAO);
				glCheckError();
				glDeleteBuffers(1, &meshComponent.VBO);
				glCheckError();
			}
		);

	m_SpinSystem = m_World.system<SpinComponent, TransformComponent>("SpinSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::iter& it, size_t, SpinComponent& spinComponent, TransformComponent& transformComponent)
			{
				transformComponent.Rotation *= glm::angleAxis(glm::degrees(0.015f * it.delta_time()), glm::vec3(0.0f, 0.0f, 1.0f));
			}
		);

	return true;
}

void Engine::Terminate()
{
	m_RenderShutdownSystem.run();

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

		ProcessInput(m_Window);

		m_RenderTarget->Bind();

		glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
		glCheckError();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();

		m_World.progress();

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