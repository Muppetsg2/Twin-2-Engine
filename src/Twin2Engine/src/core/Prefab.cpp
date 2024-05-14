#include <core/Prefab.h>

using namespace Twin2Engine::Core;

Prefab::Prefab(size_t id) : _id(id) { }

size_t Prefab::GetId() const
{
	return _id;
}

void Prefab::SetRootObject(const YAML::Node& rootObjectNode)
{
	_rootObject = rootObjectNode;
}

void Prefab::Deserialize(const YAML::Node& prefabNode)
{
	Scene::Deserialize(prefabNode);
#pragma region LOAD_ROOT_GAMEOBJECT_DATA_FROM_PREFAB_FILE
	SetRootObject(prefabNode["Root"]);
#pragma endregion
}