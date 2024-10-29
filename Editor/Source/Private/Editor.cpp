#include <Editor.h>

Editor::Editor()
{
#ifdef __linux__
	std::cout << "LINUX" << std::endl;
#endif // __linux__

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	int WIDTH = 1920;
	int HEIGHT = 1080;
	
	EditorRenderTarget* renderTarget = new EditorRenderTarget();
	m_Engine = new Engine(WIDTH, HEIGHT, "Editor", renderTarget);

	m_Engine->Init();
	m_Engine->MainLoop();
	m_Engine->Terminate();
}

Editor::~Editor()
{
	delete m_Engine;
}