#include <core/ComponentsMap.h>

using namespace std;
using namespace YAML;
using namespace Twin2Engine::Core;

hash<string> ComponentsMap::_hasher;
map<size_t, ComponentsMap::ComponentFunc> ComponentsMap::_components;
map<size_t, string> ComponentsMap::_componentsNames;

bool ComponentsMap::HasComponent(const string& type)
{
	return _components.contains(_hasher(type));
}

void ComponentsMap::AddComponent(const string& type, const ComponentsMap::ComponentFunc componentFunc)
{
	size_t typeHash = _hasher(type);
	if (_components.contains(typeHash))
	{
		SPDLOG_WARN("There is a function that creates a component with the given type '{0}'. Skip", type);
		return;
	}
	_components[typeHash] = componentFunc;
	_componentsNames[typeHash] = type;
}

void ComponentsMap::RemoveComponent(const string& type)
{
	size_t typeHash = _hasher(type);
	if (!_components.contains(typeHash))
	{
		SPDLOG_WARN("No function found to create a component with the given type '{0}'", type);
		return;
	}
	_components.erase(typeHash);
	_componentsNames.erase(typeHash);
}

Component* ComponentsMap::CreateComponent(const string& type)
{
	size_t typeHash = _hasher(type);
	if (!_components.contains(typeHash)) {
		SPDLOG_ERROR("No function found to create a component with the given type '{0}'", type);
		return nullptr;
	}
	return _components[typeHash]();
}

map<size_t, string> ComponentsMap::GetComponentsTypes()
{
	std::map<size_t, std::string> names = std::map<size_t, std::string>();

	for (auto item : _componentsNames) {
		names[item.first] = std::filesystem::path(item.second).stem().string();
	}
	return names;
}
