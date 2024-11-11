#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/SpinComponent.h>

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
	Engine engine = Engine(WIDTH, HEIGHT, "Spin", renderTarget);

	engine.Init();

	glfwSetInputMode(engine.m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	SkyboxComponent skyboxComponent;
	skyboxComponent.vertexPath = "Skybox/Skybox.vert";
	skyboxComponent.fragmentPath = "Skybox/Skybox.frag";
	skyboxComponent.texturePaths = {
		"Skybox/Right.hdr",
		"Skybox/Left.hdr",
		"Skybox/Top.hdr",
		"Skybox/Bottom.hdr",
		"Skybox/Front.hdr",
		"Skybox/Back.hdr",
	};

	flecs::entity skybox = engine.m_World.entity("Skybox")
		.set<SkyboxComponent>(skyboxComponent);

	MeshComponent meshComponent1;
	meshComponent1.vertexPath = "Cube.vert";
	meshComponent1.fragmentPath = "Cube.frag";
	meshComponent1.texturePath = "stevie-nicks.jpg";

	MeshComponent meshComponent2;
	meshComponent2.vertexPath = "Cube.vert";
	meshComponent2.fragmentPath = "Cube.frag";
	meshComponent2.texturePath = "stevie-nicks-2.jpg";

	flecs::entity cubes = engine.m_World.entity("Cubes")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, -3.0f)});
		//.set<SpinComponent>({10.0f, glm::vec3(0.0f, 0.0f, 1.0f)});

	flecs::entity stevieNicksCube = engine.m_World.entity("StevieNicksCube")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(1.0f, 0.0f, 0.0f)})
		.set<MeshComponent>(meshComponent1);
		//.set<SpinComponent>({1.23f, glm::vec3(0.0f, 1.0f, 0.0f)});

	flecs::entity stevieNicksCube2 = engine.m_World.entity("StevieNicksCube2")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(-1.0f, 0.0f, 0.0f)})
		.set<MeshComponent>(meshComponent2);
		//.set<SpinComponent>({-1.35f, glm::vec3(0.0f, 0.0f, 1.0f)});

	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f)})
		.set<PlayerMovementComponent>({5.0f})
		.set<PlayerYawComponent>({1.0f});

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f)})
		.add<CameraComponent>()
		.set<PlayerPitchComponent>({1.0f});

	flecs::system m_SpinSystem = engine.m_World.system<SpinComponent, flecs::pair<TransformComponent, Local>>("SpinSystem")
		.kind(flecs::OnUpdate)
		.each([](flecs::iter& it, size_t, SpinComponent& spinComponent, flecs::pair<TransformComponent, Local> transformComponent)
			{
				transformComponent->Rotation *= glm::angleAxis(spinComponent.speed * it.delta_time(), spinComponent.axis);
			}
		);

	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}