#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/SpinComponent.h>

//bool LoadGltfModel(std::filesystem::path path, const char* name, Engine* engine)
bool LoadGltfModel(std::filesystem::path path, const char* name, Engine* engine)
{
	fastgltf::Parser parser;

	auto data = fastgltf::GltfDataBuffer::FromPath(path);
	if (data.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf data buffer error" << std::endl;
		return false;
	}

	//auto asset = parser.loadGltf(data.get(), path.parent_path(), fastgltf::Options::None);
	auto asset = parser.loadGltfBinary(data.get(), path.parent_path(), fastgltf::Options::None);
	if (asset.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf get data error" << std::endl;
		return false;
	}
	auto& assetData = asset.get();

	flecs::entity modelEntity = engine->m_World.entity(name)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, -3.0) });
	
	for (fastgltf::Node& node : asset->nodes)
	{
		if (node.meshIndex.has_value())
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			const auto& mesh = assetData.meshes[node.meshIndex.value()];

			//for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it)
			for (auto& primitive : mesh.primitives)
			{
				auto* positionIt = primitive.findAttribute("POSITION");
				auto* normalIt = primitive.findAttribute("NORMAL");
				if (positionIt == primitive.attributes.end() || normalIt == primitive.attributes.end())
					continue;

				auto& positionAccessor = assetData.accessors[positionIt->accessorIndex];
				auto& normalAccessor = assetData.accessors[normalIt->accessorIndex];
				
				vertices.resize(positionAccessor.count);

				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(assetData, positionAccessor, [&](fastgltf::math::fvec3 position, std::size_t idx)
				{
					vertices[idx].position = glm::vec3(position.x(), position.y(), position.z());
				});


				if (normalIt == primitive.attributes.end())
					continue;
				
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(assetData, normalAccessor, [&](fastgltf::math::fvec3 normal, std::size_t idx)
				{
					vertices[idx].normal = glm::vec3(normal.x(), normal.y(), normal.z());
				});
				
				if (primitive.indicesAccessor.has_value())
				{
					auto& indexAccessor = assetData.accessors[primitive.indicesAccessor.value()];
					fastgltf::iterateAccessorWithIndex<std::uint32_t>(assetData, indexAccessor, [&](std::uint32_t idx, std::size_t index)
					{
						indices.push_back(idx);
					});
				}
			}

			MeshComponent meshComponent;
			meshComponent.vertices = vertices;
			meshComponent.indices = indices;
			meshComponent.vertexPath = "Mesh.vert";
			meshComponent.fragmentPath = "Mesh.frag";
			
			flecs::entity meshEntity = engine->m_World.entity(node.name.c_str())
				.child_of(modelEntity)
				.add<TransformComponent, Global>()
				.add<TransformComponent, Local>()
				.set<MeshComponent>(meshComponent);
		}
	}
	return true;
}

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

	std::vector<Vertex> cubeVertices =
	{
		// Positions							// Normals							// Texture Coords
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.0f,  0.0f, -1.0f),		glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.0f,  0.0f, -1.0f),		glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.5f,  0.5f, -0.5f),	glm::vec3(0.0f,  0.0f, -1.0f),		glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f),	glm::vec3(0.0f,  0.0f, -1.0f),		glm::vec2(0.0f, 1.0f)),

		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(0.0f,  0.0f,  1.0f),		glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(0.0f,  0.0f,  1.0f),		glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.5f,  0.5f,  0.5f),	glm::vec3(0.0f,  0.0f,  1.0f),		glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f),	glm::vec3(0.0f,  0.0f,  1.0f),		glm::vec2(0.0f, 1.0f)),

		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f),	glm::vec3(-1.0f,  0.0f,  0.0f),		glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f),	glm::vec3(-1.0f,  0.0f,  0.0f),		glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(-1.0f,  0.0f,  0.0f),		glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(-1.0f,  0.0f,  0.0f),		glm::vec2(0.0f, 0.0f)),

		Vertex(glm::vec3(0.5f,  0.5f,  0.5f),	glm::vec3(1.0f,  0.0f,  0.0f),		glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.5f,  0.5f, -0.5f),	glm::vec3(1.0f,  0.0f,  0.0f),		glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(1.0f,  0.0f,  0.0f),		glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(1.0f,  0.0f,  0.0f),		glm::vec2(0.0f, 0.0f)),

		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, -1.0f,  0.0f),		glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, -1.0f,  0.0f),		glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(0.0f, -1.0f,  0.0f),		glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(0.0f, -1.0f,  0.0f),		glm::vec2(0.0f, 0.0f)),

		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f),	glm::vec3(0.0f,  1.0f,  0.0f),		glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(0.5f,  0.5f, -0.5f),	glm::vec3(0.0f,  1.0f,  0.0f),		glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(0.5f,  0.5f,  0.5f),	glm::vec3(0.0f,  1.0f,  0.0f),		glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f),	glm::vec3(0.0f,  1.0f,  0.0f),		glm::vec2(0.0f, 0.0f))
	};

	std::vector<unsigned int> cubeIndices =
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	LoadGltfModel("survival_guitar_backpack.glb", "Cube", &engine);
	//LoadGltfModel("the_great_drawing_room.glb", "Cube", &engine);

	//MeshComponent meshComponent1;
	//meshComponent1.vertices = cubeVertices;
	//meshComponent1.indices = cubeIndices;
	//meshComponent1.vertexPath = "Mesh.vert";
	//meshComponent1.fragmentPath = "Mesh.frag";
	//meshComponent1.texturePath = "stevie-nicks.jpg";

	//MeshComponent meshComponent2;
	//meshComponent2.vertices = cubeVertices;
	//meshComponent2.indices = cubeIndices;
	//meshComponent2.vertexPath = "Mesh.vert";
	//meshComponent2.fragmentPath = "Mesh.frag";
	//meshComponent2.texturePath = "stevie-nicks-2.jpg";
	
	//flecs::entity cubes = engine.m_World.entity("Cubes")
		//.add<TransformComponent, Global>()
		//.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, -3.0f) })
		//.set<SpinComponent>({1.0f, glm::vec3(0.0f, 0.0f, 1.0f)});

	//flecs::entity stevieNicksCube = engine.m_World.entity("StevieNicksCube")
		//.child_of(cubes)
		//.add<TransformComponent, Global>()
		//.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, -3.0f) })
		//.set<MeshComponent>(meshComponent1);
		//.set<SpinComponent>({ 1.23f, glm::vec3(0.0f, 1.0f, 0.0f) });
	
	//flecs::entity stevieNicksCube2 = engine.m_World.entity("StevieNicksCube2")
		//.child_of(cubes)
		//.add<TransformComponent, Global>()
		//.set<TransformComponent, Local>({ glm::vec3(-1.0f, 0.0f, 0.0f) })
		//.set<MeshComponent>(meshComponent2)
		//.set<SpinComponent>({ -1.35f, glm::vec3(0.0f, 0.0f, 1.0f) });

	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.set<PlayerMovementComponent>({5.0f})
		.set<PlayerYawComponent>({ 5.0f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.set<CameraComponent>({ 50.0f })
		.set<PlayerPitchComponent>({ 5.0f });

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
