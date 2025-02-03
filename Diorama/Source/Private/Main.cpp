#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/OceanComponent.h>
#include <Components/SpinComponent.h>

void KeyCallback(Engine* engine, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetInputMode(engine->m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case GLFW_KEY_N:
			engine->globalNormalMapsEnabled = !engine->globalNormalMapsEnabled;
			break;
		case GLFW_KEY_M:
			engine->globalShadowMapsEnabled = !engine->globalShadowMapsEnabled;
			break;
		case GLFW_KEY_F11:
			if (glfwGetWindowMonitor(engine->m_Window))
			{
				glfwSetWindowMonitor(engine->m_Window, NULL, 100, 100, 1280, 720, GLFW_DONT_CARE);
			}
			else
			{
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(engine->m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			break;
		}
	}
}

void MouseButtonCallback(Engine* engine, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_1:
			glfwSetInputMode(engine->m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}
	}
}

int main()
{
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	int WIDTH = 1280;
	int HEIGHT = 720;

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	Engine engine = Engine(WIDTH, HEIGHT, "Diorama", renderTarget);

	engine.Init();

	glfwSetInputMode(engine.m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	SkyboxComponent skyboxComponent;
	skyboxComponent.vertexPath = "Skybox/Skybox.vert";
	skyboxComponent.fragmentPath = "Skybox/Skybox.frag";
	skyboxComponent.texturePaths = {
		"Skybox/Right.hdr",
		"Skybox/Left.hdr",
		"Skybox/Top.hdr",
		"Skybox/Bottom.hdr",
		"Skybox/Front.hdr",
		"Skybox/Back.hdr",
	};

	flecs::entity skybox = engine.m_World.entity("Skybox")
		.set<SkyboxComponent>(skyboxComponent);
	
	flecs::entity directionalLight = engine.m_World.entity("DirectionalLight")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(), glm::quat(glm::vec3(glm::radians(-60.0f), glm::radians(135.0f), glm::radians(0.0f))) })
		.set<BaseLightComponent>({ 0.9f, glm::vec3(1.0f, 1.0f, 1.0f), true, 8192, 8192 })
		.set<DirectionalLightComponent>({ 0.1f });
	
	//flecs::entity pointLight1 = engine.m_World.entity("PointLight1")
	//	.add<TransformComponent, Global>()
	//	.set<TransformComponent, Local>({glm::vec3(0.7f, 1.0f, 0.0f) })
	//	.set<BaseLightComponent>({ 3.0f, glm::vec3(0.0f, 0.0f, 1.0f) })
	//	.set<PointLightComponent>({ 1.0f, 1.0f, 0.5f });
	
	//flecs::entity pointLight2 = engine.m_World.entity("PointLight2")
	//	.add<TransformComponent, Global>()
	//	.set<TransformComponent, Local>({ glm::vec3(-0.7f, 1.0f, 0.0f) })
	//	.set<BaseLightComponent>({ 3.0f, glm::vec3(1.0f, 0.0f, 0.0f) })
	//	.set<PointLightComponent>({ 1.0f, 1.0f, 0.5f });
	
	//engine.GetResourceManager().LoadModel(engine.m_World, "Cube.glb", "Cube");
	//engine.GetResourceManager().LoadModel(engine.m_World, "the_great_drawing_room.glb", "GreatDrawingRoom");
	//engine.GetResourceManager().LoadModel(engine.m_World, "the_hindu_god_ganesh.glb", "God");
	//engine.GetResourceManager().LoadModel(engine.m_World, "survival_guitar_backpack.glb", "SurvivalBackpack");

	engine.GetResourceManager().LoadModel(engine.m_World, "witch_treehouse.glb", "WitchTreehouse");

	//engine.GetResourceManager().InstantiateModel(engine.m_World, "SurvivalBackpack", glm::vec3(0.0f, 3.0f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(90.0f))), glm::vec3(0.0025f, 0.0025f, 0.0025f))
		//.set<SpinComponent>({ 0.75f, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)) });
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "God", glm::vec3(0.0f, 3.0f, -0.5f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.001f, 0.001f, 0.001f))
		//.set<SpinComponent>({ 0.1f, glm::vec3(0.0f, 1.0f, 0.0f) });
	
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "Cube", glm::vec3(0.0f, -3.0f, 0.0f), glm::quat(), glm::vec3(1000.0f, 1.0f, 1000.0f));

	//engine.GetResourceManager().InstantiateModel(engine.m_World, "Cube", glm::vec3(0.0f, 0.0f, -2.0f), glm::quat(), glm::vec3(1.0f, 1.0f, 1.0f));
	
	engine.GetResourceManager().InstantiateModel(engine.m_World, "WitchTreehouse", glm::vec3(0.0f, -2.5f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.01f, 0.01f, 0.01f));
	
	OceanComponent oceanComponent;
	oceanComponent.color = glm::vec3(0.0f, 0.3f, 0.35f);
	
	flecs::entity ocean = engine.m_World.entity("Ocean")
		.add<TransformComponent, Global>()
		.add<TransformComponent, Local>()
		.set<OceanComponent>(oceanComponent);
	
	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 35.0f, 45.0f) })
		.set<PlayerMovementComponent>({ 10.0f })
		.set<PlayerYawComponent>({ 1.0f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(glm::radians(-15.0f), 0.0f, 0.0f))})
		.set<CameraComponent>({ 70.0f })
		.set<PlayerPitchComponent>({ 1.0f });
	
	engine.RegisterKeyCallback(KeyCallback);
	engine.RegisterMouseButtonCallback(MouseButtonCallback);

	flecs::system oceanInitSystem = engine.m_World.system<OceanComponent>("OceanInitSystem")
		.kind(0)
		.each([](OceanComponent& oceanComponent)
			{
				oceanComponent.shader = Shader("Ocean.vert", "Ocean.frag", "Ocean.tcs", "Ocean.tes");

				glGenVertexArrays(1, &oceanComponent.VAO);
				glGenBuffers(1, &oceanComponent.VBO);

				glBindVertexArray(oceanComponent.VAO);
				glBindBuffer(GL_ARRAY_BUFFER, oceanComponent.VBO);

				glBufferData(GL_ARRAY_BUFFER, sizeof(oceanComponent.vertices), oceanComponent.vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glBindVertexArray(0);
			}
		);
	oceanInitSystem.run();
	
	flecs::system oceanRenderSystem = engine.m_World.system<OceanComponent, flecs::pair<TransformComponent, Global>>("OceanRenderSystem")
		.kind(flecs::OnUpdate)
		.each([&](flecs::iter& iter, size_t, OceanComponent& oceanComponent, flecs::pair<TransformComponent, Global> transformComponent)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				
				int width;
				int height;
				renderTarget->GetWindowSize(width, height);

				const CameraComponent* camera = engine.m_MainCamera.get<CameraComponent>();
				const TransformComponent* cameraTransform = engine.m_MainCamera.get<TransformComponent, Global>();

				oceanComponent.shader.Use();
				
				oceanComponent.shader.SetVec3("color", oceanComponent.color);
				
				oceanComponent.shader.SetFloat("time", glfwGetTime());

				oceanComponent.shader.SetFloat("tessellationFactor", 32.0f);

				glm::mat4 projection = glm::perspective(glm::radians(camera->Fov), (float)width / (float)height, 0.01f, 1000.0f);
				oceanComponent.shader.SetMat4("projection", projection);

				glm::vec3 forward = cameraTransform->Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
				glm::vec3 up = glm::rotate(cameraTransform->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));

				glm::mat4 view = glm::lookAt(cameraTransform->Position, cameraTransform->Position + forward, up);
				oceanComponent.shader.SetMat4("view", view);
				
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, transformComponent->Position);
				model *= glm::toMat4(transformComponent->Rotation);
				model = glm::scale(model, transformComponent->Scale);
				oceanComponent.shader.SetMat4("model", model);

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				
				glPatchParameteri(GL_PATCH_VERTICES, 4);
				
				glBindVertexArray(oceanComponent.VAO);

				//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glDrawArrays(GL_PATCHES, 0, 4);

				glBindVertexArray(0);
				
				//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		);
	
	flecs::system spinSystem = engine.m_World.system<SpinComponent, flecs::pair<TransformComponent, Local>>("SpinSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::iter& iter, size_t, SpinComponent& spinComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				transformComponent->Rotation *= glm::angleAxis(spinComponent.speed * iter.delta_time(), spinComponent.axis);
			}
		);

	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}