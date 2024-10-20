#include <Editor.h>

Editor::Editor()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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
	m_Engine = new Engine(window, renderTarget);

	m_Engine->Init();
	m_Engine->MainLoop();
	m_Engine->Terminate();
}

Editor::~Editor()
{
	delete m_Engine;
}