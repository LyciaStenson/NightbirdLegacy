#pragma once

#include <TransformComponent.h>
#include <MeshComponent.h>

#include <vector>
#include <unordered_map>

#include <any>

#include <iostream>
#include <typeindex>

class NBScene
{
private:
	std::unordered_map<uint32_t, std::unordered_map<std::type_index, std::any>> Components;
	//std::vector<TransformComponent> TransformComponents;
	//std::vector<MeshComponent> MeshComponents;

public:
	template<typename T>
	void AddComponent(uint32_t entity, T component)
	{
		std::type_index typeIndex = std::type_index(typeid(T));
		Components[entity][typeIndex] = component;
		std::cout << "AddComponent > " << typeIndex.name() << std::endl;
	}

	template<typename T>
	T& GetComponent(uint32_t entity)
	{
		std::type_index typeIndex = std::type_index(typeid(T));
		std::cout << "GetComponent > " << typeIndex.name() << std::endl;
		auto entityIt = Components.find(entity);
		if (entityIt != Components.end())
		{
			auto compIt = entityIt->second.find(typeIndex);
			if (compIt != entityIt->second.end())
			{
				return std::any_cast<T&>(compIt->second);
			}
		}
		throw std::runtime_error("Component not found for entity");
	}
};