#include <Engine.h>
#include <GameRenderTarget.h>

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
		.set<TransformComponent, Local>({ glm::vec3(), glm::quat(glm::vec3(glm::radians(-55.0f), glm::radians(45.0f), glm::radians(0.0f))) })
		.set<BaseLightComponent>({ 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), true, 1024, 1024 })
		.set<DirectionalLightComponent>({ 0.05f });

	flecs::entity pointLight1 = engine.m_World.entity("PointLight1")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.7f, 1.0f, 0.0f) })
		.set<BaseLightComponent>({ 3.0f, glm::vec3(0.0f, 0.0f, 1.0f) })
		.set<PointLightComponent>({ 1.0f, 1.0f, 0.5f });
	
	flecs::entity pointLight2 = engine.m_World.entity("PointLight2")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(-0.7f, 1.0f, 0.0f) })
		.set<BaseLightComponent>({ 3.0f, glm::vec3(1.0f, 0.0f, 0.0f) })
		.set<PointLightComponent>({ 1.0f, 1.0f, 0.5f });
	
	engine.GetResourceManager().LoadModel(engine.m_World, "Cube.glb", "Cube");
	//engine.GetResourceManager().LoadModel(engine.m_World, "the_great_drawing_room.glb", "GreatDrawingRoom");
	//engine.GetResourceManager().LoadModel(engine.m_World, "the_hindu_god_ganesh.glb", "God");
	engine.GetResourceManager().LoadModel(engine.m_World, "survival_guitar_backpack.glb", "SurvivalBackpack");

	engine.GetResourceManager().InstantiateModel(engine.m_World, "Cube", glm::vec3(0.0f, -5.25f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(5.0f, 0.5f, 5.0f));
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "Cube", glm::vec3(0.7f, 1.0f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.1f, 0.1f, 0.1f));
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "Cube", glm::vec3(-0.7f, 1.0f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.1f, 0.1f, 0.1f));
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "GreatDrawingRoom", glm::vec3(0.0f, -2.5f, 0.0f), glm::quat(glm::vec3(0.0f, glm::radians(-42.0f), 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));

	engine.GetResourceManager().InstantiateModel(engine.m_World, "SurvivalBackpack", glm::vec3(0.0f, 1.0f, -0.5f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.0025f, 0.0025f, 0.0025f))
		.set<SpinComponent>({ 0.1f, glm::vec3(0.0f, 1.0f, 0.0f) });
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "God", glm::vec3(0.0f, 2.0f, -0.5f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.001f, 0.001f, 0.001f))
		//.set<SpinComponent>({ 0.1f, glm::vec3(0.0f, 1.0f, 0.0f) });
	
	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 1.0f, 3.0f) })
		.set<PlayerMovementComponent>({ 1.0f })
		.set<PlayerYawComponent>({ 1.0f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.set<CameraComponent>({ 70.0f })
		.set<PlayerPitchComponent>({ 1.0f });
	
	engine.RegisterKeyCallback(KeyCallback);
	
	flecs::system spinSystem = engine.m_World.system<SpinComponent, flecs::pair<TransformComponent, Local>>("SpinSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::iter& it, size_t, SpinComponent& spinComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				transformComponent->Rotation *= glm::angleAxis(spinComponent.speed * it.delta_time(), spinComponent.axis);
			}
		);

	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}