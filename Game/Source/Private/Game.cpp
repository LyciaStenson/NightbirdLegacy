#include <Game.h>

Game::Game()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	int WIDTH = 1280;
	int HEIGHT = 720;

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Nightbird", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GFLW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	engine = new Engine(window, renderTarget);
	engine->Init();
	engine->MainLoop();
	engine->Terminate();
	delete renderTarget;
}

Game::~Game()
{
	delete engine;
}