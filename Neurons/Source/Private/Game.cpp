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

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	m_Engine = new Engine(WIDTH, HEIGHT, "Neurons", renderTarget);

	m_Engine->Init();

	//glfwSetInputMode(m_Engine->m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	MeshComponent cubeMesh;
	cubeMesh.vertexPath = "Cube.vert";
	cubeMesh.fragmentPath = "Cube.frag";
	cubeMesh.texturePath = "stevie-nicks.jpg";

	NeuronsComponent neuronsComponent;
	neuronsComponent.vertexPath = "RayMarching.vert";
	neuronsComponent.fragmentPath = "RayMarching.frag";

	flecs::entity cube = m_Engine->m_World.entity("Cube")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, -5.0f)})
		.set<MeshComponent>(cubeMesh);

	flecs::entity neurons = m_Engine->m_World.entity("Nuerons")
		.set<NeuronsComponent>(neuronsComponent);

	flecs::entity player = m_Engine->m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f)})
		.set<PlayerMovementComponent>({5.0f})
		.set<PlayerYawComponent>({1.0f});

	flecs::entity camera = m_Engine->m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f)})
		.add<CameraComponent>()
		.set<PlayerPitchComponent>({1.0f});

	flecs::system neuronsInitSystem = m_Engine->m_World.system<NeuronsComponent>("NeuronsInitSystem")
		.kind(0)
		.each([](NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader = Shader(neuronsComponent.vertexPath, neuronsComponent.fragmentPath);
				
				glGenVertexArrays(1, &neuronsComponent.VAO);
				glGenBuffers(1, &neuronsComponent.VBO);
				glGenBuffers(1, &neuronsComponent.EBO);
				
				glBindVertexArray(neuronsComponent.VAO);

				glBindBuffer(GL_ARRAY_BUFFER, neuronsComponent.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(neuronsComponent.vertices), &neuronsComponent.vertices[0], GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, neuronsComponent.EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(neuronsComponent.indices), &neuronsComponent.indices[0], GL_STATIC_DRAW);

				// Specify the layout of the vertex data
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				
				//neuronsComponent.shader.Use();
			}
		);
	neuronsInitSystem.run();

	flecs::system neuronsRenderSystem = m_Engine->m_World.system<NeuronsComponent>("NeuronsRenderSystem")
		.kind(0)
		.each([&](const NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader.Use();

				int width;
				int height;
				glfwGetWindowSize(m_Engine->m_Window, &width, &height);

				neuronsComponent.shader.SetVec2("uResolution", width, height);

				glBindVertexArray(neuronsComponent.VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		);

	m_Engine->InitSystems();
	m_Engine->MainLoop();
	m_Engine->Terminate();
	delete renderTarget;
}

Game::~Game()
{
	delete m_Engine;
}