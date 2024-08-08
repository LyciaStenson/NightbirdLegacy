#include <editor.h>

Editor::Editor()
{
	engine = new Engine();
	engine->Init();
	engine->MainLoop();
	engine->Terminate();
}

Editor::~Editor()
{
	delete engine;
}

/*
int main()
{
	try
	{
		FileSystem::current_path("Assets");
	}
	catch(const std::exception& e)
	{
		std::cerr << "Failed to set working directory to Assets: " << e.what() << '\n';
	}
	
	Engine* engine = new Engine();
	engine->Init();

	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	//ImGui_ImplGlfw_InitForOpenGL(window, true);
	//ImGui_ImplOpenGL3_Init();

	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();

	return 0;
}
*/