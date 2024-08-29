#pragma once

#include <TransformComponent.h>
#include <MeshComponent.h>

#include <vector>
#include <unordered_map>

class NBScene
{
public:
	std::vector<TransformComponent> TransformComponents;
	std::vector<MeshComponent> MeshComponents;

	template<typename T>
	void AddComponent(uint32_t entity, T component);

	template<typename T>
	T& GetComponent(uint32_t entity);
};