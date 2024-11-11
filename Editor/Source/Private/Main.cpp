#include <Engine.h>
#include <EditorRenderTarget.h>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	int WIDTH = 1920;
	int HEIGHT = 1080;

	EditorRenderTarget* renderTarget = new EditorRenderTarget();
	Engine engine = Engine(WIDTH, HEIGHT, "Editor", renderTarget);

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
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, -3.0f)});

	flecs::entity stevieNicksCube = engine.m_World.entity("StevieNicksCube")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(1.0f, 0.0f, 0.0f)})
		.set<MeshComponent>(meshComponent);

	flecs::entity stevieNicksCube2 = engine.m_World.entity("StevieNicksCube2")
		.child_of(cubes)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(-1.0f, 0.0f, 0.0f)})
		.set<MeshComponent>(meshComponent);

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

	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}