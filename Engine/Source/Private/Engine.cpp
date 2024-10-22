#include <Engine.h>

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
	glfwSetKeyCallback(m_Window, KeyCallback);
	glfwSetCursorEnterCallback(m_Window, CursorEnterCallback);
	glfwSetCursorPosCallback(m_Window, MouseMoveCallback);

	glfwSetWindowSizeLimits(m_Window, 304, 190, GLFW_DONT_CARE, GLFW_DONT_CARE);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return false;
	}

	std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, 0);

	glEnable(GL_DEPTH_TEST);

	m_World.add<InputComponent>();

	flecs::system mainCameraInitSystem = m_World.system<CameraComponent>("MainCameraInitSystem")
		.kind(0)
		.each([&](flecs::iter& it, size_t, CameraComponent& cameraComponent)
			{
				mainCamera = it.entity(0);
			}
		);
	mainCameraInitSystem.run();

	flecs::system renderInitSystem = m_World.system<MeshComponent>("RenderInitSystem")
		.kind(0)
		.each([](MeshComponent& meshComponent)
			{
				meshComponent.shader = Shader(meshComponent.vertexPath, meshComponent.fragmentPath);

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
				unsigned char* data = stbi_load(meshComponent.texturePath, &width, &height, &nrChannels, 0);

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

	m_RenderTarget->Init();

	flecs::system renderSystem = m_World.system<TransformComponent, MeshComponent>("RenderSystem")
		.kind(flecs::OnUpdate)
		.run([&](flecs::iter& iter)
			{
				while (iter.next())
				{
					flecs::field<TransformComponent> transformComponent = iter.field<TransformComponent>(0);
					flecs::field<MeshComponent> meshComponent = iter.field<MeshComponent>(1);

					flecs::entity current = mainCamera;
					TransformComponent globalCameraTransform;
					globalCameraTransform.Position = mainCamera.get<TransformComponent>()->Position;
					globalCameraTransform.Rotation = mainCamera.get<TransformComponent>()->Rotation;
					globalCameraTransform.Scale = mainCamera.get<TransformComponent>()->Scale;

					while (flecs::entity parent = current.parent())
					{
						const TransformComponent* parentTransform = parent.get<TransformComponent>();
						if (parentTransform)
						{
							globalCameraTransform.Scale *= parentTransform->Scale;
							globalCameraTransform.Rotation *= parentTransform->Rotation;
							globalCameraTransform.Position = parentTransform->Position + (parentTransform->Rotation * (parentTransform->Scale * globalCameraTransform.Position));
						}
						current = parent;
					}

					for (auto i : iter)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, meshComponent[i].texture);
						glBindTexture(GL_TEXTURE_2D, 1);

						meshComponent[i].shader.Use();

						meshComponent[i].shader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
						meshComponent[i].shader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
						meshComponent[i].shader.SetVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
						
						meshComponent[i].shader.SetVec3("viewPos", globalCameraTransform.Position);

						int width;
						int height;
						m_RenderTarget->GetWindowSize(width, height);

						glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 1000.0f);
						meshComponent[i].shader.SetMat4("projection", projection);

						//glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f) * camera.get<TransformComponent>()->Rotation;
						glm::vec3 forward = glm::rotate(globalCameraTransform.Rotation, glm::vec3(0.0f, 0.0f, -1.0f));
						glm::vec3 up = glm::rotate(globalCameraTransform.Rotation, glm::vec3(0.0f, 1.0f, 0.0f));

						glm::mat4 view = glm::lookAt(globalCameraTransform.Position, globalCameraTransform.Position + forward, up);;
						meshComponent[i].shader.SetMat4("view", view);
						
						glBindVertexArray(meshComponent[i].VAO);

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
					}
				}
			}
		);

	m_RenderShutdownSystem = m_World.system<MeshComponent>("RenderShutdownSystem")
		.kind(flecs::OnSet)
		.each([](MeshComponent& meshComponent)
			{
				glDeleteVertexArrays(1, &meshComponent.VAO);
				glDeleteBuffers(1, &meshComponent.VBO);
			}
		);
	
	flecs::system playerInputSystem = m_World.system<PlayerInputComponent, TransformComponent>("PlayerInputSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& it, size_t, PlayerInputComponent& playerInputComponent, TransformComponent& transformComponent)
			{
				InputComponent* input = m_World.get_mut<InputComponent>();

				glm::vec3 forward = transformComponent.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
				glm::vec3 right = transformComponent.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

				glm::vec3 worldUp = glm::inverse(transformComponent.Rotation) * glm::vec3(0.0f, 1.0f, 0.0f);
				worldUp = glm::normalize(worldUp);
				
				float movement = playerInputComponent.speed * it.delta_time();

				glm::quat pitch = glm::angleAxis(input->lookY * 0.001f, glm::vec3(1.0f, 0.0f, 0.0f));
				pitch = glm::normalize(pitch);
				glm::quat yaw = glm::angleAxis(input->lookX * 0.001f, worldUp);
				yaw = glm::normalize(yaw);

				glm::quat orientation = yaw * pitch;

				transformComponent.Rotation *= glm::normalize(orientation);

				glm::vec3 eulerAngles = glm::eulerAngles(transformComponent.Rotation);
				
				std::cout << glm::degrees(eulerAngles.x) << ", " << glm::degrees(eulerAngles.y) << ", " << glm::degrees(eulerAngles.z) << std::endl;
				
				if (input->moveForward)
					transformComponent.Position += forward * movement;
				if (input->moveBackward)
					transformComponent.Position -= forward * movement;
				if (input->moveRight)
					transformComponent.Position += right * movement;
				if (input->moveLeft)
					transformComponent.Position -= right * movement;
				if (input->moveUp)
					transformComponent.Position += up * movement;
				if (input->moveDown)
					transformComponent.Position -= up * movement;

				input->lookX = 0.0f;
				input->lookY = 0.0f;
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

		fps = (int)(1.0f / deltaTime);

		m_RenderTarget->Bind();

		glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_RenderTarget->ShouldRun())
		{
			m_World.progress();
		}

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
	engine->HandleFramebuffer(width, height);
}

void Engine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
	engine->HandleKey(key, scancode, action, mods);
}

void Engine::MouseMoveCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
	engine->HandleMouseMove(window, xPosIn, yPosIn);
}

void Engine::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	//camera.ProcessMouseScroll((float)yOffset);
}

void Engine::HandleFramebuffer(int width, int height)
{
	m_RenderTarget->WindowResize(width, height);
}

void Engine::HandleKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_RELEASE)
	{
		bool isPressed = (action == GLFW_PRESS);
		InputComponent* input = m_World.get_mut<InputComponent>();
		switch (key)
		{
		case GLFW_KEY_W:
			input->moveForward = isPressed;
			break;
		case GLFW_KEY_S:
			input->moveBackward = isPressed;
			break;
		case GLFW_KEY_D:
			input->moveRight = isPressed;
			break;
		case GLFW_KEY_A:
			input->moveLeft = isPressed;
			break;
		case GLFW_KEY_E:
		case GLFW_KEY_SPACE:
			input->moveUp = isPressed;
			break;
		case GLFW_KEY_Q:
		case GLFW_KEY_LEFT_SHIFT:
			input->moveDown = isPressed;
			break;
		}
	}
}

void Engine::HandleMouseMove(GLFWwindow* window, double xPos, double yPos)
{
	float xOffset = lastX - (float)xPos;
	float yOffset = lastY - (float)yPos;

	lastX = (float)xPos;
	lastY = (float)yPos;

	InputComponent* input = m_World.get_mut<InputComponent>();
	input->lookX = xOffset;
	input->lookY = yOffset;
}

void Engine::HandleCursorEnter()
{

}

void Engine::HandleScroll()
{

}

void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cout << message << std::endl;
}