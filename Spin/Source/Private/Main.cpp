#include <Engine.h>
#include <GameRenderTarget.h>

#include <Components/SpinComponent.h>

bool LoadImage(fastgltf::Asset& asset, fastgltf::Image& image, std::vector<Texture>& textures)
{
	auto GetLevelCount = [](int width, int height) -> GLsizei
		{
			return static_cast<GLsizei>(1 + floor(log2(width > height ? width : height)));
		};

	unsigned int texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	std::visit(fastgltf::visitor
		{
			[](auto& arg) {},
			[&](fastgltf::sources::URI& filePath)
			{
				std::cout << "Image type 1" << std::endl;
			},
			[&](fastgltf::sources::Array& vector)
			{
				std::cout << "Image type 2" << std::endl;
			},
			[&](fastgltf::sources::BufferView& view)
			{
				auto& bufferView = asset.bufferViews[view.bufferViewIndex];
				auto& buffer = asset.buffers[bufferView.bufferIndex];
				std::visit(fastgltf::visitor
					{
						[](auto& arg) {},
						[&](fastgltf::sources::Array& vector)
						{
							int width, height, nrChannels;
							stbi_set_flip_vertically_on_load(true);
							unsigned char* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data() + bufferView.byteOffset),
																		static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);
							glTextureStorage2D(texture, GetLevelCount(width, height), GL_SRGB8_ALPHA8, width, height);
							glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
							stbi_image_free(data);
						}
					}, buffer.data);
			},
		}, image.data);

	glGenerateTextureMipmap(texture);
	textures.push_back(Texture({ texture }));

	return true;
}

bool LoadMaterial(fastgltf::Asset& asset, fastgltf::Material& gltfMaterial, std::vector<Material>& materials)
{
	Material material;

	material.hasBaseColorTexture = gltfMaterial.pbrData.baseColorTexture.has_value();
	
	materials.push_back(material);

	return true;
}

void IterateNode(flecs::world world, const fastgltf::Node& node, const fastgltf::Asset& assetData, const std::vector<Texture> textures, flecs::entity parentEntity)
{
	fastgltf::math::fvec3 translation(0.0f, 0.0f, 0.0f);
	fastgltf::math::fquat rotation;
	fastgltf::math::fvec3 scale(1.0f, 1.0f, 1.0f);

	if (auto* transform = std::get_if<fastgltf::math::fmat4x4>(&node.transform))
	{
		fastgltf::math::decomposeTransformMatrix(*transform, scale, rotation, translation);
	}

	int counter = 2;
	std::string name = node.name.c_str();
	while (world.lookup(name.c_str()))
	{
		name = node.name.c_str() + std::to_string(counter);
		counter++;
	}
	flecs::entity nodeEntity = world.entity(name.c_str())
		.child_of(parentEntity)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>(
			{
				glm::vec3(translation.x(), translation.y(), translation.z()),
				glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z()),
				glm::vec3(scale.x(), scale.y(), scale.z())
			});

	if (node.meshIndex.has_value())
	{
		const auto& mesh = assetData.meshes[node.meshIndex.value()];
		
		std::vector<MeshPrimitive> primitives;
		for (const auto& primitive : mesh.primitives)
		{
			MeshPrimitive meshPrimitive;

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			auto* positionIt = primitive.findAttribute("POSITION");
			auto* normalIt = primitive.findAttribute("NORMAL");
			
			if (positionIt != primitive.attributes.end())
			{
				const auto& positionAccessor = assetData.accessors[positionIt->accessorIndex];
				vertices.resize(positionAccessor.count);

				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(assetData, positionAccessor, [&](fastgltf::math::fvec3 position, std::size_t idx)
					{
						vertices[idx].position = glm::vec3(position.x(), position.y(), position.z());
					});
			}

			if (normalIt != primitive.attributes.end())
			{
				auto& normalAccessor = assetData.accessors[normalIt->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(assetData, normalAccessor, [&](fastgltf::math::fvec3 normal, std::size_t idx)
					{
						vertices[idx].normal = glm::vec3(normal.x(), normal.y(), normal.z());
					});
			}

			if (primitive.indicesAccessor.has_value())
			{
				auto& indexAccessor = assetData.accessors[primitive.indicesAccessor.value()];
				fastgltf::iterateAccessorWithIndex<std::uint32_t>(assetData, indexAccessor, [&](std::uint32_t idx, std::size_t index)
					{
						indices.push_back(idx);
					});
			}

			std::size_t baseColorTexcoordIndex = 0;
			if (primitive.materialIndex.has_value())
			{
				auto& material = assetData.materials[primitive.materialIndex.value()];
				auto& baseColorTexture = material.pbrData.baseColorTexture;
				if (baseColorTexture.has_value())
				{
					auto& texture = assetData.textures[baseColorTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						meshPrimitive.material.baseColorTexture = textures[texture.imageIndex.value()].id;
						meshPrimitive.material.hasBaseColorTexture = true;
						if (baseColorTexture->transform && baseColorTexture->transform->texCoordIndex.has_value())
						{
							baseColorTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
						}
						else
						{
							baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
						}
					}
				}
				else
				{
					auto& baseColor = material.pbrData.baseColorFactor;
					std::cout << "baseColor: " << baseColor.x() << ", " << baseColor.y() << ", " << baseColor.z() << ", " << baseColor.w() << std::endl;
					meshPrimitive.material.baseColor = glm::vec4(baseColor.x(), baseColor.y(), baseColor.z(), baseColor.w());
					meshPrimitive.material.hasBaseColorTexture = false;
				}
			}

			auto texcoordAttribute = std::string("TEXCOORD_") + std::to_string(baseColorTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(texcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = assetData.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(assetData, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							vertices[idx].texCoords = glm::vec2(uv.x(), uv.y());
						});
				}
			}

			meshPrimitive.vertices = vertices;
			meshPrimitive.indices = indices;
			meshPrimitive.material.vertexPath = "Mesh.vert";
			meshPrimitive.material.fragmentPath = "Mesh.frag";
			
			primitives.push_back(meshPrimitive);
		}

		MeshComponent meshComponent;
		meshComponent.primitives = primitives;
		
		nodeEntity.set<MeshComponent>(meshComponent);
	}
	
	for (const auto& childNodeIndex : node.children)
	{
		const auto& childNode = assetData.nodes[childNodeIndex];
		IterateNode(world, childNode, assetData, textures, nodeEntity);
	}
}

bool LoadGltfModel(flecs::world world, std::filesystem::path path, std::string name, glm::vec3 rootPosition, glm::quat rootRotation, glm::vec3 rootScale)
{
	fastgltf::Parser parser;

	auto data = fastgltf::GltfDataBuffer::FromPath(path);
	if (data.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf data buffer error" << std::endl;
		return false;
	}
	
	auto asset = parser.loadGltfBinary(data.get(), path.parent_path(), fastgltf::Options::None);
	if (asset.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf get data error" << std::endl;
		return false;
	}
	auto& assetData = asset.get();

	std::vector<Texture> textures;
	for (auto& image : assetData.images)
	{
		LoadImage(assetData, image, textures);
	}

	std::vector<Material> materials;
	for (auto& material : assetData.materials)
	{
		LoadMaterial(assetData, material, materials);
	}

	int counter = 2;
	std::string nodeName = name;
	while (world.lookup(nodeName.c_str()))
	{
		nodeName = name + std::to_string(counter);
		counter++;
	}
	flecs::entity modelEntity = world.entity(nodeName.c_str())
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ rootPosition, rootRotation, rootScale });

	std::cout << assetData.nodes.size() << " Nodes" << std::endl;
	for (const auto& rootNodeIndex : assetData.scenes[0].nodeIndices)
	{
		std::cout << "Scene" << std::endl;
		const auto& rootNode = assetData.nodes[rootNodeIndex];
		IterateNode(world, rootNode, assetData, textures, modelEntity);
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
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
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

	//LoadGltfModel(engine.m_World, "Cube.glb", "Cube", glm::vec3(0.0f, 0.0f, -3.0f), glm::quat(), glm::vec3(1.0f, 1.0f, 1.0f));
	LoadGltfModel(engine.m_World, "survival_guitar_backpack.glb", "SurvivalGuitar", glm::vec3(0.0f, 0.0f, -3.0f), glm::quat(), glm::vec3(0.002f, 0.002f, 0.002f));
	LoadGltfModel(engine.m_World, "the_great_drawing_room.glb", "GreatDrawingRoom", glm::vec3(0.0f, -2.5f, 0.0f), glm::quat(glm::vec3(0.0f, glm::radians(-42.0f), 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
	
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

	//flecs::entity cube1 = engine.m_World.entity("Cube1")
		//.child_of(cubes)
		//.add<TransformComponent, Global>()
		//.set<TransformComponent, Local>({ glm::vec3(-1.5f, 0.0f, -3.0f) })
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
		.set<PlayerMovementComponent>({ 7.0f })
		.set<PlayerYawComponent>({ 1.0f });

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.set<CameraComponent>({ 70.0f })
		.set<PlayerPitchComponent>({ 1.0f });

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
