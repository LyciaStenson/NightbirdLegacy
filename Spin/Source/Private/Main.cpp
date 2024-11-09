#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/SpinComponent.h>

int main()
{
	int WIDTH = 1280;
	int HEIGHT = 720;

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	Engine engine = Engine(WIDTH, HEIGHT, "Spin", renderTarget);

	engine.Init();

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

	MeshComponent meshComponent;
	meshComponent.vertexPath = "Cube.vert";
	meshComponent.fragmentPath = "Cube.frag";
	meshComponent.texturePath = "stevie-nicks.jpg";

	flecs::entity cubes = engine.m_World.entity("Cubes")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, -3.0f)})
		.set<SpinComponent>({10.0f, glm::vec3(0.0f, 0.0f, 1.0f)});

	flecs::entity stevieNicksCube = engine.m_World.entity("StevieNicksCube")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(1.0f, 0.0f, 0.0f)})
		.set<MeshComponent>(meshComponent)
		.set<SpinComponent>({1.23f, glm::vec3(0.0f, 1.0f, 0.0f)});

	flecs::entity stevieNicksCube2 = engine.m_World.entity("StevieNicksCube2")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(-1.0f, 0.0f, 0.0f)})
		.set<MeshComponent>(meshComponent)
		.set<SpinComponent>({-1.35f, glm::vec3(0.0f, 0.0f, 1.0f)});

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