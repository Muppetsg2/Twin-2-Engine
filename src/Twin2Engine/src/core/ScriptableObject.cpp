#include <core/ScriptableObject.h>

using namespace Twin2Engine::Core;

void Twin2Engine::Core::ScriptableObject::Serialize(YAML::Node& node)
{

}

void Twin2Engine::Core::ScriptableObject::Deserialize(const YAML::Node& node)
{
}

ScriptableObject* Twin2Engine::Core::ScriptableObject::Create()
{
	return new ScriptableObject();
}

void Twin2Engine::Core::ScriptableObject::Register()
{

}