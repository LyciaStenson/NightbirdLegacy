#include <Editor.h>

Editor::Editor()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(1280, 800, "Nightbird", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	//GameRenderTarget* renderTarget = new GameRenderTarget();
	EditorRenderTarget* renderTarget = new EditorRenderTarget(window);
	engine = new Engine(window, renderTarget);
	engine->Init();
	engine->MainLoop();
	engine->Terminate();
}

Editor::~Editor()
{
	delete engine;
}