#include <Engine.h>
#include <GameRenderTarget.h>

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
		.set<TransformComponent, Local>({ glm::vec3(), glm::quat(glm::vec3(glm::radians(-35.0f), glm::radians(45.0f), glm::radians(0.0f))) })
		.set<BaseLightComponent>({ 0.0f, glm::vec3(1.0f, 1.0f, 1.0f) })
		.set<DirectionalLightComponent>({ 0.2f });

	flecs::entity pointLight1 = engine.m_World.entity("PointLight1")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 1.0f, 0.0f) })
		.set<BaseLightComponent>({ 3.0f, glm::vec3(1.0f, 1.0f, 1.0f) })
		.set<PointLightComponent>({ 1.0f, 1.0f, 0.5f });
	
	//flecs::entity pointLight2 = engine.m_World.entity("PointLight2")
	//	.add<TransformComponent, Global>()
	//	.set<TransformComponent, Local>({ glm::vec3(-0.6f, 1.0f, 0.0f) })
	//	.set<BaseLightComponent>({ 0.0f, glm::vec3(1.0f, 0.1f, 0.1f) })
	//	.set<PointLightComponent>({ 0.0f, 0.0f, 1.0f });
	
	engine.GetResourceManager().LoadModel(engine.m_World, "Cube.glb", "Cube");
	//engine.GetResourceManager().LoadModel(engine.m_World, "the_great_drawing_room.glb", "GreatDrawingRoom");
	//engine.GetResourceManager().LoadModel(engine.m_World, "survival_guitar_backpack.glb", "SurvivalBackpack");

	engine.GetResourceManager().InstantiateModel(engine.m_World, "Cube", glm::vec3(0.0f, -0.25f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(10.0f, 0.5f, 10.0f));
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "GreatDrawingRoom", glm::vec3(0.0f, -2.5f, 0.0f), glm::quat(glm::vec3(0.0f, glm::radians(-42.0f), 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
	//engine.GetResourceManager().InstantiateModel(engine.m_World, "SurvivalBackpack", glm::vec3(0.0f, 1.0f, 0.0f), glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))), glm::vec3(0.0025f, 0.0025f, 0.0025f));
	
	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 1.0f, 3.0f) })
		.set<PlayerMovementComponent>({ 3.0f })
		.set<PlayerYawComponent>({ 1.0f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.set<CameraComponent>({ 70.0f })
		.set<PlayerPitchComponent>({ 1.0f });
	
	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}
