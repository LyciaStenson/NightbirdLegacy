#include <EditorRenderTarget.h>

EditorRenderTarget::EditorRenderTarget(GLFWwindow* aWindow)
{
	window = aWindow;
	
	width = 1280;
	height = 800;
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiStyle& style = ImGui::GetStyle();

	style.TabRounding = 8.0f;
	style.FrameRounding = 8.0f;
	style.GrabRounding = 8.0f;
	style.WindowRounding = 8.0f;
	style.PopupRounding = 8.0f;

	style.WindowPadding = ImVec2(10.0f, 10.0f);
	style.CellPadding = ImVec2(10.0f, 10.0f);

	ImVec4* colors = style.Colors;

	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);		 // Dark gray background
	//colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);			 // White text
	//colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);		 // Blue button
	//colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.8f, 1.0f); // Lighter blue when hovered
	//colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.5f, 1.0f);	 // Darker blue when pressed
	colors[ImGuiCol_Header] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);		 // Header color
	//colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.5f, 1.0f); // Header when hovered
	//colors[ImGuiCol_HeaderActive] = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);	 // Header when active
	
	ImGui::DockSpaceOverViewport(ImGui::GetID("Dockspace"));

	if (ImGui::BeginMainMenuBar())
	{
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
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				showAboutWindow = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (showAboutWindow)
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize;
		ImGui::Begin("About Nightbird", &showAboutWindow, windowFlags);
		ImGui::Text("Nightbird Dev 0.0.1");
		ImGui::Text("MIT License");
		ImGui::End();
	}

	ImGui::Begin("Scene");

	ImVec2 newSize = ImGui::GetContentRegionAvail();
	if ((int)newSize.x != width || (int)newSize.y != height)
	{
		SceneWindowResize((int)newSize.x, (int)newSize.y);
	}
	glViewport(0, 0, width, height);
	ImGui::Image((void*)(intptr_t)framebufferTexture, newSize);
	ImGui::End();

	ImGui::Begin("Entities");
	ImGui::End();

	ImGui::Begin("Components");
	if (ImGui::Button("Test"))
	{
		test++;
		std::string text = "Test " + std::to_string(test);
		Log(text);
	}
	ImGui::End();

	ImGui::Begin("File Browser");
	ImGui::End();

	ImGui::Begin("Console");
	for (std::string text : consoleText)
	{
		ImGui::Text(text.c_str());
	}
	ImGui::End();
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorRenderTarget::WindowResize(int aWidth, int aHeight)
{

}

void EditorRenderTarget::SceneWindowResize(int aWidth, int aHeight)
{
	/*
	width = aWidth;
	height = aHeight;

	std::cout << "SceneWindowResize: width >> " << width << ", height >> " << height << std::endl;

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
	*/
}

void EditorRenderTarget::Log(const std::string& text)
{
	consoleText.push_back(std::move(text));
	std::cout << text << std::endl;
}