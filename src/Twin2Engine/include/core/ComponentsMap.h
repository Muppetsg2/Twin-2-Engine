#pragma once

#include <core/GameObject.h>
#include <tools/EventHandler.h>
#include <tools/YamlConverters.h>

namespace Twin2Engine::Core {
	class ComponentsMap {
	private:
		using ComponentFunc = Tools::Func<Component*>;

		static std::hash<std::string> _hasher;
		static std::map<size_t, ComponentFunc> _components;
	public:
		static bool HasComponent(const std::string& type);
		static void AddComponent(const std::string& type, const ComponentFunc componentFunc);
		static void RemoveComponent(const std::string& type);
		static Component* CreateComponent(const std::string& type);
	};
}

#define ADD_COMPONENT(name, type)\
	ComponentsMap::AddComponent(name, \
	[]() -> Component* {\
		return new type();\
	})