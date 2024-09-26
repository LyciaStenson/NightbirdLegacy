#include <Editor.h>

Editor::Editor()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	int WIDTH = 1920;
	int HEIGHT = 1080;

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Nightbird", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	
	EditorRenderTarget* renderTarget = new EditorRenderTarget(window);
	engine = new Engine(window, renderTarget);
	renderTarget->world = &engine->m_World;
	engine->Init();
	engine->MainLoop();
	engine->Terminate();
}

Editor::~Editor()
{
	delete engine;
}