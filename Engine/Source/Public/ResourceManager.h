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
	
	flecs::entity InstantiateModel(flecs::world world, const std::string& name, const glm::vec3& rootPosition, const glm::quat& rootRotation, const glm::vec3& rootScale);

private:
	std::unordered_map<const char*, std::vector<Texture>> texturesMap;

	std::unordered_map<const char*, Shader> shadersMap;
	
	void IterateNode(flecs::world world, const fastgltf::Node& node, const fastgltf::Asset& asset, const char* modelName, flecs::entity parent);
	
	bool LoadImage(fastgltf::Asset& asset, fastgltf::Image& image, const char* modelName, TextureType textureType, bool sRGB);

	void LoadImages(fastgltf::Asset& asset, const char* modelName);
};