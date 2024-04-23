#include <core/ScriptableObject.h>

using namespace Twin2Engine::Core;

std::hash<std::string> ScriptableObject::hasher;
std::unordered_map<size_t, ScriptableObject::ScriptableObjectData> ScriptableObject::scriptableObjects;

void ScriptableObject::Serialize(YAML::Node& node) const
{
	node["__SO_RegisteredName__"] = "ScriptableObject";
	//node["id"] = _id;
}

bool ScriptableObject::Deserialize(const YAML::Node& node)
{
	//_id = node["id"].as<int>();
	return true;
}

ScriptableObject* ScriptableObject::Create()
{
	return new ScriptableObject();
}

void ScriptableObject::Register()
{

}


size_t ScriptableObject::GetId() const
{
	return _id;
}