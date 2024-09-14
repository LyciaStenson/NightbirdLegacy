#pragma once

//#include <cstdint>
//
//#include <unordered_map>
//
//#include <any>
//
//#include <iostream>
//#include <typeindex>

//using EntityId = uint32_t;
//using ComponentId = std::type_index;
//
//class NBScene
//{
//private:
//	std::unordered_map<EntityId, std::unordered_map<ComponentId, std::any>> Components;
//
//public:
//	template<typename T>
//	void AddComponent(uint32_t entity, T component)
//	{
//		std::type_index typeIndex = std::type_index(typeid(T));
//		Components[entity][typeIndex] = component;
//		//std::cout << "AddComponent > " << typeIndex.name() << std::endl;
//	}
//
//	template<typename T>
//	T& GetComponent(uint32_t entity)
//	{
//		std::type_index typeIndex = std::type_index(typeid(T));
//		//std::cout << "GetComponent > " << typeIndex.name() << std::endl;
//		auto entityIt = Components.find(entity);
//		if (entityIt != Components.end())
//		{
//			auto compIt = entityIt->second.find(typeIndex);
//			if (compIt != entityIt->second.end())
//			{
//				return std::any_cast<T&>(compIt->second);
//			}
//		}
//		throw std::runtime_error("Component not found for entity");
//	}
//};