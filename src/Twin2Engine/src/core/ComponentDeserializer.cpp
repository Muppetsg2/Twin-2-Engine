#include <core/ComponentDeserializer.h>

using namespace std;
using namespace YAML;
using namespace Twin2Engine::Core;

hash<string> ComponentDeserializer::hasher;
map<size_t, ComponentDeserializer::DeserializePair> ComponentDeserializer::_deserializerPairs;

ComponentDeserializer::DeserializePair ComponentDeserializer::GetDeserializerPair(const std::string& type)
{
	size_t typeHash = hasher(type);
	if (_deserializerPairs.find(typeHash) == _deserializerPairs.end()) {
		SPDLOG_WARN("Nie znaleziono deserializatora dla typu '{0}'", type);
		return pair(ComponentFunc(), DeserializeAction());
	}
	return _deserializerPairs[typeHash];
}

void ComponentDeserializer::AddDeserializer(const string& type, const ComponentDeserializer::ComponentFunc componentFunc, const ComponentDeserializer::DeserializeAction deserializeAction)
{
	size_t typeHash = hasher(type);
	if (_deserializerPairs.find(typeHash) != _deserializerPairs.end())
	{
		SPDLOG_WARN("Istnieje deserializator dla danego typu '{0}'", type);
		return;
	}
	_deserializerPairs[typeHash] = { componentFunc, deserializeAction };
}

bool ComponentDeserializer::HasDeserializer(const string& type)
{
	return _deserializerPairs.find(hasher(type)) != _deserializerPairs.end();
}

ComponentDeserializer::ComponentFunc ComponentDeserializer::GetComponentFunc(const string& type)
{
	return GetDeserializerPair(type).first;
}

ComponentDeserializer::DeserializeAction ComponentDeserializer::GetDeserializeAction(const string& type)
{
	return GetDeserializerPair(type).second;
}

void ComponentDeserializer::RemoveDeserializer(const string& type)
{
	size_t typeHash = hasher(type);
	if (_deserializerPairs.find(typeHash) != _deserializerPairs.end())
	{
		SPDLOG_WARN("Nie znaleziono deserializatora dla typu '{0}'", type);
		return;
	}
	_deserializerPairs.erase(typeHash);
}