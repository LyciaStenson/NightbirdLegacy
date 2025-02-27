#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/NeuronsComponent.h>

int main()
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
	Engine engine = Engine(WIDTH, HEIGHT, "Neurons", renderTarget);

	engine.Init();
	
	glfwSetInputMode(engine.m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	NeuronsComponent neuronsComponent;
	neuronsComponent.vertexPath = "Neurons.vert";
	neuronsComponent.fragmentPath = "Neurons.frag";
	neuronsComponent.neuronPositions =
	{
		15.0f,  4.0f,  7.0f,	// 0
		11.0f,  7.0f,  2.0f,	// 1
		-5.0f,  8.0f,  7.0f,	// 2
		11.0f,  2.0f, -6.0f,	// 3
		 0.0f,  0.0f,  5.0f,	// 4
		-5.0f, -3.0f,  7.0f,	// 5
		-3.0f,  7.0f,  2.0f,	// 6
		 5.0f, -5.0f, -3.0f,	// 7
	   -15.0f, -9.0f, -7.0f		// 8
	};

	flecs::entity neurons = engine.m_World.entity("Nuerons")
		.set<NeuronsComponent>(neuronsComponent);

	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 25.0f) })
		.set<PlayerMovementComponent>({ 10.0f })
		.set<PlayerYawComponent>({ 0.5f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.add<CameraComponent>()
		.set<PlayerPitchComponent>({ 0.5f });

	flecs::system neuronsInitSystem = engine.m_World.system<NeuronsComponent>("NeuronsInitSystem")
		.kind(0)
		.each([&](NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader.Load(neuronsComponent.vertexPath, neuronsComponent.fragmentPath);
				neuronsComponent.shader.Use();

				neuronsComponent.shader.SetFloatArray("uNeuronPositions", neuronsComponent.neuronPositions);
				neuronsComponent.shader.SetInt("uNeuronPositionsSize", neuronsComponent.neuronPositions.size());
				
				std::vector<int> connections =
				{
					0, 1,
					0, 3,
					0, 4,
					1, 2,
					1, 3,
					1, 4,
					2, 3,
					2, 4,
					3, 4,
					3, 5,
					3, 6,
					4, 5,
					4, 6,
					4, 7,
					5, 6,
					6, 7,
					6, 8,
					7, 8,
				};

				neuronsComponent.shader.SetIntArray("uConnections", connections);
				neuronsComponent.shader.SetInt("uConnectionsSize", connections.size());

				//std::cout << "uConnectionsSize: " << connections.size() << std::endl;

				//std::cout << "i = 0; i < uConnectionsSize - 2: " << (0 <= connections.size() - 2) << std::endl;

				glGenVertexArrays(1, &neuronsComponent.VAO);
				glGenBuffers(1, &neuronsComponent.VBO);
				glGenBuffers(1, &neuronsComponent.EBO);

				glBindVertexArray(neuronsComponent.VAO);

				glBindBuffer(GL_ARRAY_BUFFER, neuronsComponent.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(neuronsComponent.vertices), &neuronsComponent.vertices, GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, neuronsComponent.EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(neuronsComponent.indices), &neuronsComponent.indices, GL_STATIC_DRAW);

				// Specify the layout of the vertex data
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}
		);
	neuronsInitSystem.run();

	flecs::system neuronsRenderSystem = engine.m_World.system<NeuronsComponent>("NeuronsRenderSystem")
		.kind(flecs::OnUpdate)
		.each([&](const NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader.Use();

				neuronsComponent.shader.SetFloat("uTime", glfwGetTime());

				const TransformComponent* cameraTransform = engine.m_MainCamera.get<TransformComponent, Global>();

				glm::vec3 forward = cameraTransform->Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
				glm::vec3 right = cameraTransform->Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
				glm::vec3 up = glm::rotate(cameraTransform->Rotation, glm::vec3(0.0f, 1.0f, 0.0f));

				neuronsComponent.shader.SetVec3("uCameraPosition", cameraTransform->Position);
				neuronsComponent.shader.SetVec3("uCameraForward", forward);
				neuronsComponent.shader.SetVec3("uCameraRight", right);
				neuronsComponent.shader.SetVec3("uCameraUp", up);

				int width = 1280;
				int height = 720;
				glfwGetWindowSize(engine.m_Window, &width, &height);

				neuronsComponent.shader.SetVec2("uResolution", width, height);

				glBindVertexArray(neuronsComponent.VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		);

	engine.InitSystems();

	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}
