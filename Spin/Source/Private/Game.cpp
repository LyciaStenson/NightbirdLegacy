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

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Spin Game", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GFLW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	m_Engine = new Engine(window, renderTarget);

	flecs::entity parent = m_Engine->m_World.entity("ParentTest");
	parent.add<TransformComponent>();
	parent.add<SpinComponent>();

	parent.set<TransformComponent>({ glm::vec3(), glm::quat(), glm::vec3(1.0f) });
	parent.set<SpinComponent>({ 1.5f });

	flecs::entity stevieNicksCube = m_Engine->m_World.entity("StevieNicksCube")
		.child_of(parent);
	stevieNicksCube.add<TransformComponent>();
	stevieNicksCube.add<MeshComponent>();
	stevieNicksCube.add<SpinComponent>();

	stevieNicksCube.set<TransformComponent>({ glm::vec3(-1.0f, 0.0f, 0.0f), glm::quat(), glm::vec3(1.0f) });
	stevieNicksCube.set<SpinComponent>({ -1.0f });

	flecs::entity stevieNicksCube2 = m_Engine->m_World.entity("StevieNicksCube2")
		.child_of(parent);
	stevieNicksCube2.add<TransformComponent>();
	stevieNicksCube2.add<MeshComponent>();
	stevieNicksCube2.add<SpinComponent>();

	stevieNicksCube2.set<TransformComponent>({ glm::vec3(1.0f, 0.0f, 0.0f), glm::quat(), glm::vec3(1.0f) });
	stevieNicksCube2.set<SpinComponent>({ 1.0f });

	flecs::entity camera = m_Engine->m_World.entity("Camera");
	camera.add<TransformComponent>();
	camera.add<CameraComponent>();
	camera.add<PlayerInputComponent>();

	camera.set<TransformComponent>({ glm::vec3(0.0f, 0.0f, -3.0f) });

	flecs::system m_SpinSystem = m_Engine->m_World.system<SpinComponent, TransformComponent>("SpinSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::iter& it, size_t, SpinComponent& spinComponent, TransformComponent& transformComponent)
			{
				transformComponent.Rotation *= glm::angleAxis(glm::degrees(0.01f * spinComponent.speed * it.delta_time()), glm::vec3(0.0f, 0.0f, 1.0f));
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