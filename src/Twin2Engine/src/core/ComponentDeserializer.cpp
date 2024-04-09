#include <core/ComponentDeserializer.h>

using namespace std;
using namespace YAML;
using namespace Twin2Engine::Core;

map<size_t, Action<GameObject*, const Node&>> ComponentDeserializer::_deserializers = map<size_t, Action<GameObject*, const Node&>>();

void ComponentDeserializer::AddDeserializer(const string& type, const Action<GameObject*, const Node&> deselializer)
{
	size_t h = hash<string>()(type);
	if (_deserializers.find(h) != _deserializers.end()) 
	{
		SPDLOG_WARN("Zastêpowanie deserializatora '{0}'", type);
	}
	_deserializers[h] = deselializer;
}

bool ComponentDeserializer::HasDeserializer(const string& type)
{
	size_t h = hash<string>()(type);
	return _deserializers.find(h) != _deserializers.end();
}

Action<GameObject*, const Node&> ComponentDeserializer::GetDeserializer(const string& type)
{
	size_t h = hash<string>()(type);
	if (_deserializers.find(h) != _deserializers.end())
	{
		SPDLOG_WARN("Nie znaleziono deserializatora dla typu '{0}'", type);
		return Action<GameObject*, const Node&>();
	}
	return _deserializers[h];
}

void ComponentDeserializer::RemoveDeserializer(const string& type)
{
	size_t h = hash<string>()(type);
	if (_deserializers.find(h) != _deserializers.end())
	{
		SPDLOG_WARN("Nie znaleziono deserializatora dla typu '{0}'", type);
		return;
	}
	_deserializers.erase(h);
}