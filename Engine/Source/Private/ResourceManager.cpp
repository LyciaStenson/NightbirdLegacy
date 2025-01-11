#include <ResourceManager.h>

bool ResourceManager::LoadModel(flecs::world world, const std::filesystem::path& path, const std::string& name)
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

	//std::vector<Texture> textures;
	for (auto& image : assetData.images)
	{
		LoadImage(assetData, image, name.c_str());
	}

	//std::vector<Material> materials;
	for (auto& material : assetData.materials)
	{
		LoadMaterial(assetData, material, name.c_str());
	}

	//int counter = 2;
	//std::string prefabName = name + "Prefab";
	//while (world.lookup(prefabName.c_str()))
	//{
		//prefabName = name + "Prefab" + std::to_string(counter);
		//counter++;
	//}
	
	flecs::entity modelPrefab = world.prefab((name + "Prefab").c_str())
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(), glm::quat(), glm::vec3(1.0f) });
	//std::cout << "Prefab name: " << name + "Prefab" << std::endl;
	for (const auto& rootNodeIndex : assetData.scenes[0].nodeIndices)
	{
		const auto& rootNode = assetData.nodes[rootNodeIndex];
		IterateNode(world, rootNode, assetData, name.c_str(), modelPrefab);
	}
	return true;
}

void ResourceManager::SpawnModelEntities(flecs::world world, const std::string& name, const glm::vec3& rootPosition, const glm::quat& rootRotation, const glm::vec3& rootScale)
{
	flecs::entity prefab = world.lookup((name + "Prefab").c_str());
	if (prefab.is_valid())
	{
		int counter = 2;
		std::string modelName = name;
		while (world.lookup(modelName.c_str()))
		{
			modelName = name + std::to_string(counter);
			counter++;
		}

		flecs::entity modelEntity = world.entity(modelName.c_str())
			.is_a(prefab)
			.set<TransformComponent, Local>({ rootPosition, rootRotation, rootScale });
		//std::cout << "Instantiated " << name + "Prefab" << " as " << modelName << std::endl;
	}
}

//fastgltf::Asset& ResourceManager::LoadNewModel(const std::string& path)
//{

//}

void ResourceManager::IterateNode(flecs::world world, const fastgltf::Node& node, const fastgltf::Asset& assetData, const char* modelName, flecs::entity parent)
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
	flecs::entity nodePrefab = world.prefab(name.c_str())
		.child_of(parent)
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
					std::cout << baseColorTexture->textureIndex << std::endl;
					auto& texture = assetData.textures[baseColorTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						meshPrimitive.material.baseColorTexture = texturesMap[modelName][texture.imageIndex.value()].id;
						meshPrimitive.material.hasBaseColorTexture = true;
						//std::cout << "Setting " << name << " baseColorTexture: " << textures[texture.imageIndex.value()].id << std::endl;
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

		nodePrefab.set<MeshComponent>(meshComponent);
	}

	for (const auto& childNodeIndex : node.children)
	{
		const auto& childNode = assetData.nodes[childNodeIndex];
		IterateNode(world, childNode, assetData, modelName, nodePrefab);
	}
}

bool ResourceManager::LoadMaterial(fastgltf::Asset& asset, fastgltf::Material& gltfMaterial, const char* modelName)
{
	Material material;

	material.hasBaseColorTexture = gltfMaterial.pbrData.baseColorTexture.has_value();

	materialsMap[modelName].push_back(material);
	
	return true;
}

bool ResourceManager::LoadImage(fastgltf::Asset& asset, fastgltf::Image& image, const char* modelName)
{
	auto GetLevelCount = [](int width, int height) -> int
		{
			return static_cast<int>(1 + floor(log2(width > height ? width : height)));
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
	texturesMap[modelName].push_back(Texture{ texture });
	//std::cout << "Loaded texture " << texture << std::endl;
	//textures.push_back(Texture({ texture }));
	
	return true;
}