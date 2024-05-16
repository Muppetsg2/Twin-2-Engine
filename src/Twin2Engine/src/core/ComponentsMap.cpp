#include <core/ComponentsMap.h>

using namespace std;
using namespace YAML;
using namespace Twin2Engine::Core;

hash<string> ComponentsMap::_hasher;
map<size_t, ComponentsMap::ComponentFunc> ComponentsMap::_components;

bool ComponentsMap::HasComponent(const string& type)
{
	return _components.contains(_hasher(type));
}

void ComponentsMap::AddComponent(const string& type, const ComponentsMap::ComponentFunc componentFunc)
{
	size_t typeHash = _hasher(type);
	if (_components.contains(typeHash))
	{
		SPDLOG_WARN("Istnieje funkcja tworz¹ca komponent o podanym typie '{0}'. Pomijanie", type);
		return;
	}
	_components[typeHash] = componentFunc;
}

void ComponentsMap::RemoveComponent(const string& type)
{
	size_t typeHash = _hasher(type);
	if (!_components.contains(typeHash))
	{
		SPDLOG_WARN("Nie znaleziono funkcji tworz¹cej komponent o podanym typie '{0}'", type);
		return;
	}
	_components.erase(typeHash);
}

Component* ComponentsMap::CreateComponent(const string& type)
{
	size_t typeHash = _hasher(type);
	if (!_components.contains(typeHash)) {
		SPDLOG_ERROR("Nie znaleziono funkcji tworz¹cej komponent o podanym typie '{0}'", type);
		return nullptr;
	}
	return _components[typeHash]();
}