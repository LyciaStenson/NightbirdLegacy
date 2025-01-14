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
	
	LoadImages(assetData, name.c_str());
	
	int counter = 2;
	std::string prefabName = name;
	while (world.lookup(prefabName.c_str()))
	{
		prefabName = name.c_str() + std::to_string(counter);
		counter++;
	}
	flecs::entity modelPrefab = world.prefab((name + "Prefab").c_str())
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(), glm::quat(), glm::vec3(1.0f) });
	
	for (const auto& rootNodeIndex : assetData.scenes[0].nodeIndices)
	{
		const auto& rootNode = assetData.nodes[rootNodeIndex];
		IterateNode(world, rootNode, assetData, name.c_str(), modelPrefab);
	}
	return true;
}

void ResourceManager::InstantiateModel(flecs::world world, const std::string& name, const glm::vec3& rootPosition, const glm::quat& rootRotation, const glm::vec3& rootScale)
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
	}
}

void ResourceManager::IterateNode(flecs::world world, const fastgltf::Node& node, const fastgltf::Asset& asset, const char* modelName, flecs::entity parent)
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
	std::string separator = "::";
	std::string path = parent.path().c_str() + separator + name;
	while (world.lookup(path.c_str()))
	{
		name = node.name.c_str() + std::to_string(counter);
		path = parent.path().c_str() + separator + name;
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
		const auto& mesh = asset.meshes[node.meshIndex.value()];

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
				const auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
				vertices.resize(positionAccessor.count);

				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, positionAccessor, [&](fastgltf::math::fvec3 position, std::size_t verticeIndex)
					{
						vertices[verticeIndex].position = glm::vec3(position.x(), position.y(), position.z());
					});
			}

			if (normalIt != primitive.attributes.end())
			{
				auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, normalAccessor, [&](fastgltf::math::fvec3 normal, std::size_t verticeIndex)
					{
						vertices[verticeIndex].normal = glm::vec3(normal.x(), normal.y(), normal.z());
					});
			}

			if (primitive.indicesAccessor.has_value())
			{
				auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];
				fastgltf::iterateAccessorWithIndex<std::uint32_t>(asset, indexAccessor, [&](std::uint32_t index, std::size_t verticeIndex)
					{
						indices.push_back(index);
					});
			}

			std::size_t baseColorTexcoordIndex = 0;
			std::size_t metallicRoughnessTexcoordIndex = 0;
			std::size_t normalTexcoordIndex = 0;
			if (primitive.materialIndex.has_value())
			{
				auto& material = asset.materials[primitive.materialIndex.value()];

				auto& baseColorFactor = material.pbrData.baseColorFactor;
				meshPrimitive.material.baseColorFactor = glm::vec4(baseColorFactor.x(), baseColorFactor.y(), baseColorFactor.z(), baseColorFactor.w());
				meshPrimitive.material.hasBaseColorTexture = false;
				auto& baseColorTexture = material.pbrData.baseColorTexture;
				if (baseColorTexture.has_value())
				{
					auto& texture = asset.textures[baseColorTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						meshPrimitive.material.baseColorTexture = texturesMap[modelName][texture.imageIndex.value()].id;
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
				
				auto& metallicRoughnessTexture = material.pbrData.metallicRoughnessTexture;
				meshPrimitive.material.metallicFactor = material.pbrData.metallicFactor;
				meshPrimitive.material.roughnessFactor = material.pbrData.roughnessFactor;
				meshPrimitive.material.hasMetallicRoughnessTexture = false;
				if (metallicRoughnessTexture.has_value())
				{
					auto& texture = asset.textures[metallicRoughnessTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						meshPrimitive.material.metallicRoughnessTexture = texturesMap[modelName][texture.imageIndex.value()].id;
						meshPrimitive.material.hasMetallicRoughnessTexture = true;
						if (metallicRoughnessTexture->transform && metallicRoughnessTexture->transform->texCoordIndex.has_value())
						{
							metallicRoughnessTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
						}
						else
						{
							metallicRoughnessTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
						}
					}
				}

				auto& normalTexture = material.normalTexture;
				meshPrimitive.material.hasNormalTexture = false;
				if (normalTexture.has_value())
				{
					auto& texture = asset.textures[normalTexture->textureIndex];
					if (texture.imageIndex.has_value())
					{
						meshPrimitive.material.normalTexture = texturesMap[modelName][texture.imageIndex.value()].id;
						meshPrimitive.material.hasNormalTexture = true;
						if (normalTexture->transform && normalTexture->transform->texCoordIndex.has_value())
						{
							normalTexcoordIndex = normalTexture->transform->texCoordIndex.value();
						}
						else
						{
							normalTexcoordIndex = material.normalTexture->texCoordIndex;
						}
					}
				}
			}
			
			auto baseColorTexcoordAttribute = std::string("TEXCOORD_") + std::to_string(baseColorTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(baseColorTexcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = asset.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							vertices[idx].baseColorTexCoords = glm::vec2(uv.x(), uv.y());
						});
				}
			}

			auto metallicRoughnessTexcoordAttribute = std::string("TEXCOORD_") + std::to_string(metallicRoughnessTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(metallicRoughnessTexcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = asset.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							vertices[idx].metallicRoughnessTexCoords = glm::vec2(uv.x(), uv.y());
						});
				}
			}

			auto normalTexcoordAttribute = std::string("TEXCOORD_") + std::to_string(normalTexcoordIndex);
			if (const auto* texcoord = primitive.findAttribute(normalTexcoordAttribute); texcoord != primitive.attributes.end())
			{
				auto& texcoordAccessor = asset.accessors[texcoord->accessorIndex];
				if (texcoordAccessor.bufferViewIndex.has_value())
				{
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texcoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx)
						{
							vertices[idx].normalTexCoords = glm::vec2(uv.x(), uv.y());
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
		const auto& childNode = asset.nodes[childNodeIndex];
		IterateNode(world, childNode, asset, modelName, nodePrefab);
	}
}

bool ResourceManager::LoadImage(fastgltf::Asset& asset, fastgltf::Image& image, const char* modelName, bool sRGB)
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
				std::cout << "External GLTF textures not yet supported. Please use GLB models." << std::endl;
			},
			[&](fastgltf::sources::Array& vector)
			{
				std::cout << "GLTF textures not yet supported. Please use GLB models." << std::endl;
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
							stbi_set_flip_vertically_on_load(false);
							unsigned char* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data() + bufferView.byteOffset),
																		static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);
							glTextureStorage2D(texture, GetLevelCount(width, height), sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8, width, height);
							glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
							stbi_image_free(data);
						}
					}, buffer.data);
			},
		}, image.data);
	
	glGenerateTextureMipmap(texture);
	texturesMap[modelName].push_back(Texture{ texture });
	
	return true;
}

void ResourceManager::LoadImages(fastgltf::Asset& asset, const char* modelName)
{
	for (auto& image : asset.images)
	{
		bool sRGB = false;
		for (auto& material : asset.materials)
		{
			if (material.pbrData.baseColorTexture.has_value()
				&& asset.textures[material.pbrData.baseColorTexture.value().textureIndex].imageIndex == &image - &asset.images[0])
			{
				sRGB = true;
			}
		}

		LoadImage(asset, image, modelName, sRGB);
	}
}