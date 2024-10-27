#include <Game.h>

Game::Game()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	int WIDTH = 1280;
	int HEIGHT = 720;

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Spin", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GFLW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	m_Engine = new Engine(window, renderTarget);

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

	flecs::entity skybox = m_Engine->m_World.entity("Skybox")
		.set<SkyboxComponent>(skyboxComponent);

	flecs::entity cubes = m_Engine->m_World.entity("Cubes")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, -3.0f) })
		.set<SpinComponent>({ 1.5f, glm::vec3(0.0f, 0.0f, 1.0f) });

	MeshComponent meshComponent;
	meshComponent.vertexPath = "Cube.vert";
	meshComponent.fragmentPath = "Cube.frag";
	meshComponent.texturePath = "stevie-nicks.jpg";

	flecs::entity stevieNicksCube = m_Engine->m_World.entity("StevieNicksCube")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(1.0f, 0.0f, 0.0f) })
		.set<MeshComponent>(meshComponent)
		.set<SpinComponent>({ 1.23f, glm::vec3(0.0f, 1.0f, 0.0f) });
	
	flecs::entity stevieNicksCube2 = m_Engine->m_World.entity("StevieNicksCube2")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(-1.0f, 0.0f, 0.0f) })
		.set<MeshComponent>(meshComponent)
		.set<SpinComponent>({ -1.35f, glm::vec3(0.0f, 0.0f, 1.0f) });
	
	flecs::entity camera = m_Engine->m_World.entity("Camera")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.add<CameraComponent>()
		.set<PlayerInputComponent>({ 5.0f });

	flecs::system m_SpinSystem = m_Engine->m_World.system<SpinComponent, flecs::pair<TransformComponent, Local>>("SpinSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::iter& it, size_t, SpinComponent& spinComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				transformComponent->Rotation *= glm::angleAxis(spinComponent.speed * it.delta_time(), spinComponent.axis);
			}
		);

	m_Engine->Init();

	m_Engine->MainLoop();
	m_Engine->Terminate();
	delete renderTarget;
}

Game::~Game()
{
	delete m_Engine;
}