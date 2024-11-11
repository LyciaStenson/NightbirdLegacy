#include <EditorRenderTarget.h>

EditorRenderTarget::EditorRenderTarget()
{
}

EditorRenderTarget::~EditorRenderTarget()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorRenderTarget::Init(GLFWwindow* window)
{
	m_Window = window;

	glfwGetWindowSize(window, &m_Width, &m_Height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.Fonts->AddFontFromFileTTF("Fonts/RobotoFlex-Regular.ttf", 16.0f);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	glDeleteFramebuffers(1, &m_Framebuffer);
	glDeleteTextures(1, &m_FramebufferTexture);
	glDeleteRenderbuffers(1, &m_Rbo);

	glGenFramebuffers(1, &m_Framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	glGenTextures(1, &m_FramebufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

	glGenRenderbuffers(1, &m_Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer is not complete" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EditorRenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
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
	colors[ImGuiCol_Header] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
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
			glfwSetWindowShouldClose(m_Window, true);
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
			m_ShowEntitiesWindow = true;
		}
		if (ImGui::MenuItem("Components"))
		{
			m_ShowComponentsWindow = true;
		}
		if (ImGui::MenuItem("Asset Browser"))
		{
			m_ShowAssetBrowserWindow = true;
		}
		if (ImGui::MenuItem("Console"))
		{
			m_ShowConsoleWindow = true;
		}
		if (ImGui::MenuItem("Scene"))
		{
			m_ShowSceneWindow = true;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About"))
		{
			m_ShowAboutWindow = true;
		}
		ImGui::EndMenu();
	}

	int availableWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(availableWidth / 2.0f);
	
	ImGui::BeginDisabled(m_shouldRun);
	if (ImGui::Button("Play"))
	{
		Log("Play");
		m_shouldRun = true;
	}
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!m_shouldRun);

	if (ImGui::Button("Stop"))
	{
		Log("Stop");
		m_shouldRun = false;
	}
	ImGui::EndDisabled();

	ImGui::EndMainMenuBar();

	if (m_ShowAboutWindow)
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 10.0f));
		ImGui::Begin("About Nightbird", &m_ShowAboutWindow, windowFlags);
		ImGui::Text("Nightbird Dev 0.1.0");
		ImGui::Text("MIT License");
		ImGui::End();
		ImGui::PopStyleVar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (m_ShowSceneWindow)
	{
		ImGui::Begin("Scene", &m_ShowSceneWindow);

		ImVec2 newSize = ImGui::GetContentRegionAvail();
		if ((int)newSize.x != m_SceneWidth || (int)newSize.y != m_SceneHeight)
		{
			m_SceneWidth = newSize.x;
			m_SceneHeight = newSize.y;
		}
		ImGui::Image((void*)(intptr_t)m_FramebufferTexture, newSize, {0, 1}, {1, 0});
		ImGui::End();
	}
	ImGui::PopStyleVar();

	if (m_ShowEntitiesWindow)
	{
		ImGui::Begin("Entities", &m_ShowEntitiesWindow);

		//flecs::query<TransformComponent> query = world->query<TransformComponent>();

		//query.each([](flecs::entity& entity, TransformComponent& transformComponent)
		//	{
		//		std::cout << "Entity >> " << entity.name() << std::endl;
		//		ImGui::Selectable(entity.name());
		//	}
		//);
		
		ImGui::End();
	}

	if (m_ShowComponentsWindow)
	{
		ImGui::Begin("Components", &m_ShowComponentsWindow);
		if (ImGui::Button("Test"))
		{
			Log("Test");
		}
		ImGui::End();
	}

	if (m_ShowAssetBrowserWindow)
	{
		ImGui::Begin("Asset Browser", &m_ShowAssetBrowserWindow);
		ImGui::End();
	}

	if (m_ShowConsoleWindow)
	{
		ImGui::Begin("Console", &m_ShowConsoleWindow);
		ImGui::BeginGroup();
		for (std::string text : m_ConsoleText)
		{
			ImGui::Text(text.c_str());
		}
		ImGui::EndGroup();
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorRenderTarget::GetWindowSize(int& width, int& height)
{
	width = m_SceneWidth;
	height = m_SceneHeight;
}

void EditorRenderTarget::WindowResize(int width, int height)
{
	m_Width = width;
	m_Height = height;
}

bool EditorRenderTarget::ShouldRun()
{
	return m_shouldRun;
}

void EditorRenderTarget::Log(const std::string& text)
{
	m_ConsoleText.push_back(text);
	std::cout << text << std::endl;
}