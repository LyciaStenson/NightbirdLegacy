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

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Neurons", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GFLW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	m_Engine = new Engine(window, renderTarget);

	flecs::entity camera = m_Engine->m_World.entity("Camera");
	camera.add<TransformComponent>();
	camera.add<CameraComponent>();
	camera.add<PlayerInputComponent>();

	camera.set<TransformComponent>({ glm::vec3(0.0f, 0.0f, -3.0f) });

	m_Engine->Init();

	m_Engine->MainLoop();
	m_Engine->Terminate();
	delete renderTarget;
}

Game::~Game()
{
	delete m_Engine;
}