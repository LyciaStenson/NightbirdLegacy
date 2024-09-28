#include <EditorRenderTarget.h>

EditorRenderTarget::EditorRenderTarget(GLFWwindow* aWindow)
{
	window = aWindow;

	engine = (Engine*)glfwGetWindowUserPointer(aWindow);

	glfwGetWindowSize(window, &width, &height);
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

EditorRenderTarget::~EditorRenderTarget()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorRenderTarget::Init()
{
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &framebufferTexture);
	glDeleteRenderbuffers(1, &rbo);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer is not complete" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EditorRenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
}

void EditorRenderTarget::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}

void EditorRenderTarget::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiStyle& style = ImGui::GetStyle();

	style.TabRounding = 4.0f;
	style.FrameRounding = 4.0f;
	//style.GrabRounding = 8.0f;
	style.WindowRounding = 6.0f;
	//style.PopupRounding = 8.0f;

	style.WindowPadding = ImVec2(10.0f, 10.0f);
	style.FramePadding = ImVec2(12.0f, 8.0f);
	//style.CellPadding = ImVec2(150.0f, 150.0f);
	style.ItemSpacing = ImVec2(10.0f, 8.0f);

	style.TabBarBorderSize = 0.0f;
	style.WindowBorderSize = 0.0f;

	ImVec4* colors = style.Colors;
	
	colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_Text] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	colors[ImGuiCol_Button] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	////colors[ImGuiCol_Header] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	////colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	////colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
	//colors[ImGuiCol_MenuBarBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

	colors[ImGuiCol_Tab] = colors[ImGuiCol_TitleBg];
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_WindowBg];
	colors[ImGuiCol_TabSelected] = colors[ImGuiCol_WindowBg];
	colors[ImGuiCol_TabSelectedOverline] = colors[ImGuiCol_WindowBg];
	colors[ImGuiCol_TabDimmed] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabDimmedSelected] = colors[ImGuiCol_TabSelected];
	colors[ImGuiCol_TabDimmedSelectedOverline] = colors[ImGuiCol_TabSelectedOverline];

	ImGui::DockSpaceOverViewport(ImGui::GetID("Dockspace"));

	ImGui::BeginMainMenuBar();
	if(ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New", "Ctrl+N"))
		{
			Log("New");
		}
		if (ImGui::MenuItem("Open", "Ctrl+O"))
		{
			Log("Open");
		}
		if (ImGui::MenuItem("Save", "Ctrl+S"))
		{
			Log("Save");
		}
		if (ImGui::MenuItem("Exit", "Ctrl+Q"))
		{
			glfwSetWindowShouldClose(window, true);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Undo", "Ctrl+Z"))
		{
			Log("Undo");
		}
		if (ImGui::MenuItem("Redo", "Ctrl+Y"))
		{
			Log("Undo");
		}
		if (ImGui::MenuItem("Cut", "Ctrl+X"))
		{
			Log("Cut");
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C"))
		{
			Log("Copy");
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V"))
		{
			Log("Paste");
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::MenuItem("Entities"))
		{
			showEntitiesWindow = true;
		}
		if (ImGui::MenuItem("Components"))
		{
			showComponentsWindow = true;
		}
		if (ImGui::MenuItem("Asset Browser"))
		{
			showAssetBrowserWindow = true;
		}
		if (ImGui::MenuItem("Console"))
		{
			showConsoleWindow = true;
		}
		if (ImGui::MenuItem("Scene"))
		{
			showSceneWindow = true;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About"))
		{
			showAboutWindow = true;
		}
		ImGui::EndMenu();
	}

	int availableWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(availableWidth / 2.0f);

	if (ImGui::Button("Play"))
	{
		Log("Play");
	}
	if (ImGui::Button("Pause"))
	{
		Log("Pause");
	}
	if (ImGui::Button("Stop"))
	{
		Log("Stop");
	}
	ImGui::EndMainMenuBar();

	if (showAboutWindow)
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 10.0f));
		ImGui::Begin("About Nightbird", &showAboutWindow, windowFlags);
		ImGui::Text("Nightbird Dev 0.1.0");
		ImGui::Text("MIT License");
		ImGui::End();
		ImGui::PopStyleVar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (showSceneWindow)
	{
		ImGui::Begin("Scene", &showSceneWindow);

		ImVec2 newSize = ImGui::GetContentRegionAvail();
		if ((int)newSize.x != sceneWidth || (int)newSize.y != sceneHeight)
		{
			sceneWidth = newSize.x;
			sceneHeight = newSize.y;
		}
		ImGui::Image((void*)(intptr_t)framebufferTexture, newSize);
		ImGui::End();
	}
	ImGui::PopStyleVar();

	if (showEntitiesWindow)
	{
		ImGui::Begin("Entities", &showEntitiesWindow);

		//flecs::query<TransformComponent> query = world->query<TransformComponent>();

		//query.each([](flecs::entity& entity, TransformComponent& transformComponent)
		//	{
		//		std::cout << "Entity >> " << entity.name() << std::endl;
		//		ImGui::Selectable(entity.name());
		//	}
		//);
		
		ImGui::End();
	}

	if (showComponentsWindow)
	{
		ImGui::Begin("Components", &showComponentsWindow);
		if (ImGui::Button("Test"))
		{
			Log("Test");
		}
		ImGui::End();
	}

	if (showAssetBrowserWindow)
	{
		ImGui::Begin("Asset Browser", &showAssetBrowserWindow);
		ImGui::End();
	}

	if (showConsoleWindow)
	{
		ImGui::Begin("Console", &showConsoleWindow);
		ImGui::BeginGroup();
		for (std::string text : consoleText)
		{
			ImGui::Text(text.c_str());
		}
		ImGui::EndGroup();
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorRenderTarget::GetWindowSize(int& aWidth, int& aHeight)
{
	aWidth = sceneWidth;
	aHeight = sceneHeight;
}

void EditorRenderTarget::WindowResize(int aWidth, int aHeight)
{
	width = aWidth;
	height = aHeight;
}

void EditorRenderTarget::Log(const std::string& text)
{
	consoleText.push_back(text);
	std::cout << text << std::endl;
}