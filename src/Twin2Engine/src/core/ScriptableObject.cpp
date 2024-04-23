#include <core/ScriptableObject.h>

using namespace Twin2Engine::Core;

std::hash<std::string> ScriptableObject::hasher;
std::unordered_map<size_t, ScriptableObject::ScriptableObjectData> ScriptableObject::scriptableObjects;

void ScriptableObject::Serialize(YAML::Node& node) const
{

}

bool ScriptableObject::Deserialize(const YAML::Node& node)
{
	return true;
}

ScriptableObject* ScriptableObject::Create()
{
	return new ScriptableObject();
}

void ScriptableObject::Register()
{

}