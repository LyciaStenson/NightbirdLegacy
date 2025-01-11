#pragma once

#include <glad/gl.h>

#include <iostream>

#include <string.h>
#include <unordered_map>

#include <stb_image.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include <flecs/flecs.h>

#include <Material.h>
#include <Texture.h>

#include <TransformComponent.h>
#include <MeshComponent.h>

class ResourceManager
{
public:
	ResourceManager() = default;

	bool LoadModel(flecs::world world, const std::filesystem::path& path, const std::string& name);

	void SpawnModelEntities(flecs::world world, const std::string& name, const glm::vec3& rootPosition, const glm::quat& rootRotation, const glm::vec3& rootScale);

private:
	std::unordered_map<const char*, std::vector<Texture>> texturesMap;
	std::unordered_map<const char*, std::vector<Material>> materialsMap;
	
	void IterateNode(flecs::world world, const fastgltf::Node& node, const fastgltf::Asset& assetData, const char* modelName, flecs::entity parent);
	
	bool LoadMaterial(fastgltf::Asset& asset, fastgltf::Material& gltfMaterial, const char* modelName);
	bool LoadImage(fastgltf::Asset& asset, fastgltf::Image& image, const char* modelName);
};