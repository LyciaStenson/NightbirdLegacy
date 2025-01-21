#include <Engine.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

using namespace winrt::Windows::UI::ViewManagement;

inline BOOL IsColorLight(winrt::Windows::UI::Color& clr)
{
	return (((5 * clr.G) + (2 * clr.R) + clr.B) > (8 * 128));
}
#endif // _WIN32

Engine::Engine(int width, int height, const char* name, RenderTarget* renderTarget)
{
	m_Window = glfwCreateWindow(width, height, name, NULL, NULL);

	if (m_Window == NULL)
	{
		std::cout << "Failed to create GFLW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(0);

	m_RenderTarget = renderTarget;
	
	m_PointLightQuery = m_World.query<BaseLightComponent, PointLightComponent, flecs::pair<TransformComponent, Global>>();
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

#ifdef _WIN32
	HWND hwnd = glfwGetWin32Window(m_Window);
	UISettings settings = UISettings();
	auto foreground = settings.GetColorValue(UIColorType::Foreground);
	BOOL isDarkMode = IsColorLight(foreground);
	DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));
#endif // _WIN32

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return false;
	}

	std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, 0);

	m_RenderTarget->Init(m_Window);
	
	glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);

	return true;
}

void Engine::InitSystems()
{
	m_GlobalTransformQuery = m_World.query_builder<const TransformComponent, const TransformComponent*, TransformComponent>()
		.term_at(0).second<Local>()
		.term_at(1).second<Global>()
		.term_at(2).second<Global>()
		.term_at(1)
		.parent().cascade()
		.build();

	m_World.add<InputComponent>();

	flecs::system mainCameraInitSystem = m_World.system<CameraComponent>("MainCameraInitSystem")
		.kind(0)
		.each([&](flecs::entity entity, CameraComponent& cameraComponent)
			{
				m_MainCamera = entity;
			}
		);
	mainCameraInitSystem.run();
	
	flecs::system directionalLightInitSystem = m_World.system<BaseLightComponent, DirectionalLightComponent>("DirectionalLightInitSystem")
		.kind(0)
		.each([&](flecs::entity entity, BaseLightComponent& lightComponent, DirectionalLightComponent& directionalLightComponent)
			{
				m_DirectionalLight = entity;
			}
		);
	directionalLightInitSystem.run();
	
	flecs::system meshInitSystem = m_World.system<MeshComponent>("MeshInitSystem")
		.kind(0)
		.each([](flecs::entity entity, MeshComponent& meshComponent)
			{
				for (auto& primitive : meshComponent.primitives)
				{
					primitive.material.shader = Shader(primitive.material.vertexPath, primitive.material.fragmentPath);

					primitive.material.shader.Use();

					primitive.material.shader.SetVec4("baseColorFactor", primitive.material.baseColorFactor);
					primitive.material.shader.SetBool("hasBaseColorTexture", primitive.material.hasBaseColorTexture);
					if (primitive.material.hasBaseColorTexture)
					{
						glBindTextureUnit(0, primitive.material.baseColorTexture);
						primitive.material.shader.SetInt("baseColorTexture", 0);
					}

					primitive.material.shader.SetFloat("metallicFactor", primitive.material.metallicFactor);
					primitive.material.shader.SetFloat("roughnessFactor", primitive.material.roughnessFactor);
					primitive.material.shader.SetBool("hasMetallicRoughnessTexture", primitive.material.hasMetallicRoughnessTexture);
					if (primitive.material.hasMetallicRoughnessTexture)
					{
						glBindTextureUnit(1, primitive.material.metallicRoughnessTexture);
						primitive.material.shader.SetInt("metallicRoughnessTexture", 1);
					}

					primitive.material.shader.SetBool("hasNormalTexture", primitive.material.hasNormalTexture);
					if (primitive.material.hasNormalTexture)
					{
						glBindTextureUnit(2, primitive.material.normalTexture);
						primitive.material.shader.SetInt("normalTexture", 2);
					}
					
					glGenVertexArrays(1, &primitive.VAO);
					glGenBuffers(1, &primitive.VBO);
					glGenBuffers(1, &primitive.EBO);

					glBindVertexArray(primitive.VAO);
					glBindBuffer(GL_ARRAY_BUFFER, primitive.VBO);

					glBufferData(GL_ARRAY_BUFFER, primitive.vertices.size() * sizeof(Vertex), &primitive.vertices[0], GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.EBO);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices.size() * sizeof(unsigned int), &primitive.indices[0], GL_STATIC_DRAW);

					// Position Attribute
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
					glEnableVertexAttribArray(0);

					// Normal Attribute
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
					glEnableVertexAttribArray(1);

					// Tangent Attribute
					glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
					glEnableVertexAttribArray(1);

					// Base Color Texture Coord Attribute
					glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, baseColorTexCoords));
					glEnableVertexAttribArray(3);

					// Metallic Roughness Texture Coord Attribute
					glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, metallicRoughnessTexCoords));
					glEnableVertexAttribArray(4);

					// Normal Texture Coord Attribute
					glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normalTexCoords));
					glEnableVertexAttribArray(5);

					glBindVertexArray(0);
				}

				//entity.set<TextureLoadComponent>({ std::async(LoadTexture, meshComponent.texturePath, false) });
			}
		);
	meshInitSystem.run();

	/*
	flecs::system meshLateInitSystem = m_World.system<MeshComponent, TextureLoadComponent>("MeshLateInitSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::entity entity, MeshComponent& meshComponent, TextureLoadComponent& textureLoadComponent)
			{
				if (textureLoadComponent.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
				{
					glGenTextures(1, &meshComponent.texture);
					glBindTexture(GL_TEXTURE_2D, meshComponent.texture);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					TextureData textureData = textureLoadComponent.future.get();

					if (textureData.data)
					{
						glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, textureData.width, textureData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data);
						glGenerateMipmap(GL_TEXTURE_2D);
						stbi_image_free(textureData.data);
						entity.remove<TextureLoadComponent>();
					}
					else
					{
						std::cout << "Failed to load texture" << std::endl;
					}

					meshComponent.shader.Use();
					meshComponent.shader.SetInt("ourTexture", 0);
				}
			}
		);
	*/

	flecs::system skyboxInitSystem = m_World.system<SkyboxComponent>("SkyboxInitSystem")
		.kind(0)
		.each([](flecs::entity entity, SkyboxComponent& skyboxComponent)
			{
				skyboxComponent.shader = Shader(skyboxComponent.vertexPath, skyboxComponent.fragmentPath);

				glGenVertexArrays(1, &skyboxComponent.VAO);
				glGenBuffers(1, &skyboxComponent.VBO);
				glGenBuffers(1, &skyboxComponent.EBO);
				glBindVertexArray(skyboxComponent.VAO);
				glBindBuffer(GL_ARRAY_BUFFER, skyboxComponent.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxComponent.vertices), &skyboxComponent.vertices, GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxComponent.EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxComponent.indices), &skyboxComponent.indices, GL_STATIC_DRAW);

				// Position Attribute
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				CubemapLoadComponent cubemapLoadComponent;
				for (unsigned int i = 0; i < skyboxComponent.texturePaths.size(); i++)
				{
					cubemapLoadComponent.futures.push_back(std::async(LoadTexture, skyboxComponent.texturePaths[i], false));
				}
				entity.set<CubemapLoadComponent>(cubemapLoadComponent);

				glActiveTexture(GL_TEXTURE0);
				glGenTextures(1, &skyboxComponent.texture);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxComponent.texture);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}
		);
	skyboxInitSystem.run();
	
	flecs::system skyboxLateInitSystem = m_World.system<CubemapLoadComponent, SkyboxComponent>("SkyboxLateInitSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::entity entity, CubemapLoadComponent& cubemapLoadComponent, SkyboxComponent& skyboxComponent)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxComponent.texture);
				for (unsigned int i = 0; i < 6; i++)
				{
					if (!cubemapLoadComponent.loadedChecks[i] && cubemapLoadComponent.futures[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready)
					{
						TextureData textureData = cubemapLoadComponent.futures[i].get();
						if (textureData.data)
						{
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, textureData.width, textureData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data);
							stbi_image_free(textureData.data);

							cubemapLoadComponent.loadedChecks[i] = true;
							
							skyboxComponent.shader.Use();
							skyboxComponent.shader.SetInt("skybox", 0);
							
							bool fullyCompleted = true;
							for (bool check : cubemapLoadComponent.loadedChecks)
							{
								if (!check)
								{
									fullyCompleted = false;
								}
							}
							if (fullyCompleted)
							{
								entity.remove<CubemapLoadComponent>();
							}
						}
						else
						{
							std::cout << "Cubemap texture failed to load: " << skyboxComponent.texturePaths[i] << std::endl;
						}
					}
				}
			}
		);

	flecs::system meshRenderSystem = m_World.system<flecs::pair<TransformComponent, Global>, MeshComponent>("MeshRenderSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& iter, size_t index, flecs::pair<TransformComponent, Global> transformComponent, MeshComponent& meshComponent)
			{
				const CameraComponent* camera = m_MainCamera.get<CameraComponent>();
				const TransformComponent* cameraTransform = m_MainCamera.get<TransformComponent, Global>();
				
				glm::vec3 directionalLightDir;
				float directionalLightIntensity = 0.0f;
				float directionalLightAmbient = 0.0f;
				glm::vec3 directionalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);

				if (m_DirectionalLight.is_valid())
				{
					const TransformComponent* transform = m_DirectionalLight.get<TransformComponent, Global>();
					directionalLightDir = glm::rotate(transform->Rotation, glm::vec3(0.0f, 0.0f, -1.0f));
					
					const BaseLightComponent* baseLight = m_DirectionalLight.get<BaseLightComponent>();
					directionalLightIntensity = baseLight->intensity;
					directionalLightColor = baseLight->color;

					const DirectionalLightComponent* directionalLight = m_DirectionalLight.get<DirectionalLightComponent>();
					directionalLightAmbient = directionalLight->ambient;
				}
				
				std::vector<TransformComponent> pointLightTransforms; // Transform can't be reference or pointer due to flecs::pair
				std::vector<BaseLightComponent*> pointBaseLightComponents;
				std::vector<PointLightComponent*> pointLightComponents;
				m_PointLightQuery.each([&](flecs::entity pointLight, BaseLightComponent& lightComponent, PointLightComponent& pointLightComponent, flecs::pair<TransformComponent, Global> transformComponent)
					{
						pointLightTransforms.push_back(transformComponent);
						pointBaseLightComponents.push_back(&lightComponent);
						pointLightComponents.push_back(&pointLightComponent);
					});
				
				for (auto& primitive : meshComponent.primitives)
				{
					glBindTextureUnit(0, primitive.material.baseColorTexture);
					
					primitive.material.shader.Use();
					
					if (m_DirectionalLight.is_valid())
					{
						primitive.material.shader.SetVec3("directionalLight.direction", directionalLightDir);
						primitive.material.shader.SetFloat("directionalLight.intensity", directionalLightIntensity);
						primitive.material.shader.SetFloat("directionalLight.ambient", directionalLightAmbient);
						primitive.material.shader.SetVec3("directionalLight.color", directionalLightColor);
					}
					
					for (int i = 0; i < pointLightTransforms.size() && i < pointBaseLightComponents.size() && i < pointLightComponents.size() && i < 16; i++)
					{
						const std::string name = "pointLights[" + std::to_string(i) + "]";
						primitive.material.shader.SetVec3(name + ".position", pointLightTransforms[i].Position);
						primitive.material.shader.SetFloat(name + ".intensity", pointBaseLightComponents[i]->intensity);
						primitive.material.shader.SetVec3(name + ".color", pointBaseLightComponents[i]->color);
						primitive.material.shader.SetFloat(name + ".constantAttenuation", pointLightComponents[i]->constantAttenuation);
						primitive.material.shader.SetFloat(name + ".linearAttenuation", pointLightComponents[i]->linearAttenuation);
						primitive.material.shader.SetFloat(name + ".quadraticAttenuation", pointLightComponents[i]->quadraticAttenuation);
					}
					if (pointLightTransforms.size() < pointBaseLightComponents.size())
					{
						primitive.material.shader.SetInt("pointLightCount", pointLightTransforms.size());
					}
					else
					{
						primitive.material.shader.SetInt("pointLightCount", pointBaseLightComponents.size());
					}
					
					primitive.material.shader.SetVec3("viewPos", cameraTransform->Position);

					int width;
					int height;
					m_RenderTarget->GetWindowSize(width, height);

					glm::mat4 projection = glm::perspective(glm::radians(camera->Fov), (float)width / (float)height, 0.01f, 1000.0f);
					primitive.material.shader.SetMat4("projection", projection);

					glm::vec3 forward = cameraTransform->Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
					glm::vec3 up = glm::rotate(cameraTransform->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));

					glm::mat4 view = glm::lookAt(cameraTransform->Position, cameraTransform->Position + forward, up);
					primitive.material.shader.SetMat4("view", view);

					glBindVertexArray(primitive.VAO);
					
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, transformComponent->Position);
					model *= glm::toMat4(transformComponent->Rotation);
					model = glm::scale(model, transformComponent->Scale);
					primitive.material.shader.SetMat4("model", model);

					glDrawElements(GL_TRIANGLES, primitive.indices.size(), GL_UNSIGNED_INT, 0);

					glBindVertexArray(0);
				}
			}
		);
	
	flecs::system skyboxRenderSystem = m_World.system<SkyboxComponent>("SkyboxRenderSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& iter, size_t index, SkyboxComponent& skyboxComponent)
			{
				const CameraComponent* camera = m_MainCamera.get<CameraComponent>();
				const TransformComponent* cameraTransform = m_MainCamera.get<TransformComponent, Global>();

				glDepthFunc(GL_LEQUAL);

				skyboxComponent.shader.Use();

				int width;
				int height;
				m_RenderTarget->GetWindowSize(width, height);

				glm::mat4 projection = glm::perspective(glm::radians(camera->Fov), (float)width / (float)height, 0.1f, 1000.0f);
				skyboxComponent.shader.SetMat4("projection", projection);

				glm::vec3 forward = cameraTransform->Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
				glm::vec3 up = cameraTransform->Rotation * glm::vec3(0.0f, 1.0f, 0.0f);

				glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(cameraTransform->Position, cameraTransform->Position + forward, up)));
				skyboxComponent.shader.SetMat4("view", view);

				glBindVertexArray(skyboxComponent.VAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxComponent.texture);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				glDepthFunc(GL_LESS);
			}
		);
	
	m_RenderShutdownSystem = m_World.system<MeshComponent>("RenderShutdownSystem")
		.kind(0)
		.each([](MeshComponent& meshComponent)
			{
				for (auto& primitive : meshComponent.primitives)
				{
					glDeleteVertexArrays(1, &primitive.VAO);
					glDeleteBuffers(1, &primitive.VBO);
				}
			}
		);

	flecs::system playerMovementSystem = m_World.system<PlayerMovementComponent, flecs::pair<TransformComponent, Local>>("PlayerMovementSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& iter, size_t index, PlayerMovementComponent& playerMovementComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				const InputComponent* input = m_World.get<InputComponent>();

				glm::vec3 forward = transformComponent->Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
				glm::vec3 right = transformComponent->Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

				float movement = playerMovementComponent.speed * iter.delta_time();

				if (input->keyW)
					transformComponent->Position += forward * movement;
				if (input->keyS)
					transformComponent->Position -= forward * movement;
				if (input->keyD)
					transformComponent->Position += right * movement;
				if (input->keyA)
					transformComponent->Position -= right * movement;
				if (input->keyE)
					transformComponent->Position += up * movement;
				if (input->keyQ)
					transformComponent->Position -= up * movement;
			}
		);

	flecs::system playerYawSystem = m_World.system<PlayerYawComponent, flecs::pair<TransformComponent, Local>>("PlayerYawSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& iter, size_t index, PlayerYawComponent& playerYawComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				InputComponent* input = m_World.get_mut<InputComponent>();

				glm::quat yaw = glm::angleAxis(input->mouseX * 0.005f * playerYawComponent.sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
				yaw = glm::normalize(yaw);

				transformComponent->Rotation *= yaw;

				input->mouseX = 0.0f;
			}
		);

	flecs::system playerPitchSystem = m_World.system<PlayerPitchComponent, flecs::pair<TransformComponent, Local>>("PlayerPitchSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& iter, size_t index, PlayerPitchComponent& playerPitchComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				InputComponent* input = m_World.get_mut<InputComponent>();

				glm::vec3 right = transformComponent->Rotation * glm::vec3(1.0f, 0.0f, 0.0f);

				glm::quat pitch = glm::angleAxis(input->mouseY * 0.005f * playerPitchComponent.sensitivity, right);
				pitch = glm::normalize(pitch);

				transformComponent->Rotation *= pitch;

				glm::vec3 eulerAngles = glm::eulerAngles(transformComponent->Rotation);
				eulerAngles.x = glm::clamp(eulerAngles.x, glm::radians(-90.0f), glm::radians(90.0f));
				transformComponent->Rotation = glm::quat(eulerAngles);

				input->mouseY = 0.0f;
			}
		);
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
		
		//std::cout << "FPS: " << fps << std::endl;

		//std::string title = "Nightbird | " + std::to_string(fps) + " FPS";
		//glfwSetWindowTitle(m_Window, title.c_str());

		m_GlobalTransformQuery
			.each([](const TransformComponent& transform, const TransformComponent* parentTransform, TransformComponent& transformOut)
				{
					transformOut.Scale = transform.Scale;
					transformOut.Rotation = transform.Rotation;
					transformOut.Position = transform.Position;
					if (parentTransform)
					{
						glm::vec3 scaledPosition = parentTransform->Scale * transform.Position;

						glm::vec3 rotatedPosition = parentTransform->Rotation * scaledPosition;

						transformOut.Position = parentTransform->Position + rotatedPosition;

						transformOut.Rotation = parentTransform->Rotation * transform.Rotation;

						transformOut.Scale = parentTransform->Scale * transform.Scale;
					}
				}
			);

		m_RenderTarget->Bind();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

ResourceManager& Engine::GetResourceManager()
{
	return m_ResourceManager;
}

TextureData Engine::LoadTexture(const char* path, bool flipVertically)
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flipVertically);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	return TextureData({ data, width, height });
}

void Engine::CursorEnterCallback(GLFWwindow* window, int entered)
{
	//if (entered)
	//{
	//	firstMouse = true;

	//	int width, height;
	//	glfwGetWindowSize(window, &width, &height);
	//	glfwSetCursorPos(window, width / 2.0f, height / 2.0f);
	//}
}

void Engine::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
	engine->HandleFramebufferSize(width, height);
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

void Engine::HandleFramebufferSize(int width, int height)
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
			input->keyW = isPressed;
			break;
		case GLFW_KEY_S:
			input->keyS = isPressed;
			break;
		case GLFW_KEY_D:
			input->keyD = isPressed;
			break;
		case GLFW_KEY_A:
			input->keyA = isPressed;
			break;
		case GLFW_KEY_E:
			input->keyE = isPressed;
			break;
		case GLFW_KEY_Q:
			input->keyQ = isPressed;
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
	input->mouseX = xOffset;
	input->mouseY = yOffset;
}

void Engine::HandleCursorEnter()
{

}

void Engine::HandleScroll()
{

}

void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "GL_DEBUG_SEVERITY_HIGH" << std::endl;
		std::cout << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "GL_DEBUG_SEVERITY_MEDIUM" << std::endl;
		std::cout << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "GL_DEBUG_SEVERITY_LOW" << std::endl;
		std::cout << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		//std::cout << "GL_DEBUG_SEVERITY_NOTIFICATION" << std::endl;
		break;
	default:
		std::cout << "OpenGL Error Unknown severity" << std::endl;
		break;
	}
}