#define RGFW_IMPLEMENTATION
#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/NeuronsComponent.h>

int main()
{
	int WIDTH = 1280;
	int HEIGHT = 720;

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	Engine engine = Engine(WIDTH, HEIGHT, "Neurons", renderTarget);

	engine.Init();

	NeuronsComponent neuronsComponent;
	neuronsComponent.vertexPath = "RayMarching.vert";
	neuronsComponent.fragmentPath = "RayMarching.frag";

	flecs::entity neurons = engine.m_World.entity("Nuerons")
		.set<NeuronsComponent>(neuronsComponent);

	float startTime;

	flecs::system neuronsInitSystem = engine.m_World.system<NeuronsComponent>("NeuronsInitSystem")
		.kind(0)
		.each([&](NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader.Load(neuronsComponent.vertexPath, neuronsComponent.fragmentPath);

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

				startTime = (float)RGFW_getTimeNS() * 0.000000001f;
			}
		);
	neuronsInitSystem.run();

	flecs::system neuronsRenderSystem = engine.m_World.system<NeuronsComponent>("NeuronsRenderSystem")
		.kind(flecs::OnUpdate)
		.each([&](const NeuronsComponent& neuronsComponent)
			{
				neuronsComponent.shader.Use();

				//neuronsComponent.shader.SetFloat("uTime", RGFW_getTimeNS() - startTime / 1000000000.0f);

				//std::cout << "Time: " << ((float)RGFW_getTimeNS() * 0.000000001f) << std::endl;
				//std::cout << "Time: " << RGFW_getTimeNS() << std::endl;

				int width = 1280;
				int height = 720;
				//glfwGetWindowSize(engine.m_Window, &width, &height);
				//RGFW_getScreenSize(engine.m_Window, width, height);

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