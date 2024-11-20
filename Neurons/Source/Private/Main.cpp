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
		 0.0f,  3.0f,  0.0f,
		 0.0f, -3.0f,  0.0f,
		 3.0f,  0.0f,  0.0f,
		-3.0f,  0.0f,  0.0f
	};

	flecs::entity neurons = engine.m_World.entity("Nuerons")
		.set<NeuronsComponent>(neuronsComponent);

	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 10.0f) })
		.set<PlayerMovementComponent>({ 5.0f })
		.set<PlayerYawComponent>({ 1.0f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.add<CameraComponent>()
		.set<PlayerPitchComponent>({ 1.0f });

	flecs::system neuronsInitSystem = engine.m_World.system<NeuronsComponent>("NeuronsInitSystem")
		.kind(0)
		.each([&](NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader.Load(neuronsComponent.vertexPath, neuronsComponent.fragmentPath);
				neuronsComponent.shader.Use();

				neuronsComponent.shader.SetFloatArray("uNeuronPositions", neuronsComponent.neuronPositions);
				neuronsComponent.shader.SetInt("uNeuronPositionsSize", neuronsComponent.neuronPositions.size());
				
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

				const TransformComponent* cameraTransform = engine.mainCamera.get<TransformComponent, Global>();

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