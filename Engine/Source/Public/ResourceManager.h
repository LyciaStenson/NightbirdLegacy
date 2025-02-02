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
	
	flecs::entity InstantiateModel(flecs::world world, const std::string& name, const glm::vec3& rootPosition = glm::vec3(), const glm::quat& rootRotation = glm::quat(), const glm::vec3& rootScale = glm::vec3(1.0f));

private:
	std::unordered_map<const char*, std::unordered_map<size_t, Texture>> texturesMap;

	std::unordered_map<const char*, Shader> shadersMap;
	
	void IterateNode(flecs::world world, const fastgltf::Node& node, const fastgltf::Asset& asset, const char* modelName, flecs::entity parent);
	
	bool LoadImage(fastgltf::Asset& asset, fastgltf::Image& image, const char* modelName, size_t textureIndex, TextureType textureType, bool sRGB);

	void LoadImages(fastgltf::Asset& asset, const char* modelName);
};