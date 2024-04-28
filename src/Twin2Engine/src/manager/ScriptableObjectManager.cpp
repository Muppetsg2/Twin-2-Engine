#include <manager/ScriptableObjectManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace std;

hash<std::string> ScriptableObjectManager::_hasher;
unordered_map<size_t, ScriptableObject*> ScriptableObjectManager::_scriptableObjects;

unordered_map<size_t, string> ScriptableObjectManager::_scriptableObjectsPaths;

unordered_map<size_t, unsigned int> ScriptableObjectManager::_serializationContext;
unordered_map<unsigned int, size_t> ScriptableObjectManager::_deserializationContext;

ScriptableObject* ScriptableObjectManager::Load(const std::string& path)
{
	size_t pathHash = _hasher(path);
	if (_scriptableObjects.contains(pathHash)) {
		SPDLOG_WARN("ScriptableObject '{0}' already loaded", path);
		return _scriptableObjects[pathHash];
	}

	SPDLOG_INFO("Loading ScriptableObject '{0}'", path);

	YAML::Node soNode = YAML::LoadFile(path)["ScriptableObject"];
	size_t hashedSORegisteredName = _hasher(soNode["__SO_RegisteredName__"].as<string>());
	ScriptableObject* scriptableObject = ScriptableObject::scriptableObjects[hashedSORegisteredName].createSpecificScriptableObject();
	scriptableObject->_id = pathHash;

	scriptableObject->Deserialize(soNode);

	_scriptableObjects[pathHash] = scriptableObject;
	_scriptableObjectsPaths[pathHash] = path;

	return scriptableObject;
}

ScriptableObject* ScriptableObjectManager::Get(const std::string& path)
{
	size_t pathHash = _hasher(path);
	if (_scriptableObjects.contains(pathHash)) {
		//SPDLOG_WARN("ScriptableObject '{0}' already loaded", path);
		return _scriptableObjects[pathHash];
	}
	SPDLOG_WARN("ScriptableObject '{0}' not loaded", path);
	return nullptr;
}

ScriptableObject* ScriptableObjectManager::Get(size_t id)
{
	if (_scriptableObjects.contains(id))
	{
		//SPDLOG_WARN("ScriptableObject '{0}' already loaded", id);
		return _scriptableObjects[id];
	}
	SPDLOG_WARN("ScriptableObject '{0}' not loaded", id);
	return nullptr;
}

void Twin2Engine::Manager::ScriptableObjectManager::UnloadAll()
{
	for (auto& pair : _scriptableObjects)
	{
		delete pair.second;
	}
}

YAML::Node Twin2Engine::Manager::ScriptableObjectManager::Serialize()
{
	YAML::Node node;

	for (const auto& pair : _serializationContext)
	{
		node[pair.second]["path"] = _scriptableObjectsPaths[pair.first];
	}
	return node;
}

void Twin2Engine::Manager::ScriptableObjectManager::SceneSerializationBegin()
{
	_serializationContext.clear();
}

void Twin2Engine::Manager::ScriptableObjectManager::SceneSerializationEnd()
{
	_serializationContext.clear();
}

unsigned int Twin2Engine::Manager::ScriptableObjectManager::SceneSerialize(size_t id)
{
	if (_serializationContext.contains(id))
	{
		return _serializationContext[id];
	}
	unsigned int index = _serializationContext.size();
	_serializationContext[id] = index;
	return index;
}

void Twin2Engine::Manager::ScriptableObjectManager::SceneDeserializationBegin()
{
	_deserializationContext.clear();
}

size_t Twin2Engine::Manager::ScriptableObjectManager::SceneDeserialize(unsigned int sceneSerializationId, const std::string& path)
{
	if (_deserializationContext.contains(sceneSerializationId))
	{
		return _deserializationContext[sceneSerializationId];
	}
	size_t hashedPath = _hasher(path);
	_deserializationContext[sceneSerializationId] = hashedPath;
	return hashedPath;
}

Twin2Engine::Core::ScriptableObject* Twin2Engine::Manager::ScriptableObjectManager::Deserialize(unsigned int sceneSerializationId)
{
	if (_deserializationContext.contains(sceneSerializationId))
	{
		return _scriptableObjects[_deserializationContext[sceneSerializationId]];
	}
	return nullptr;
}


void Twin2Engine::Manager::ScriptableObjectManager::SceneDeserializationEnd()
{
	_deserializationContext.clear();
}

vector<string> ScriptableObjectManager::GetScriptableObjectsNames()
{
	vector<string> scriptableObjectNames(ScriptableObject::scriptableObjects.size());
	size_t index = 0;
	for (auto& pair : ScriptableObject::scriptableObjects)
	{
		scriptableObjectNames[index++] = pair.second.scriptableObjectName;
	}
	return scriptableObjectNames;
}

bool Twin2Engine::Manager::ScriptableObjectManager::CreateScriptableObject(const string& dstPath, const string& scriptableObjectClassName)
{
	size_t hashedName = _hasher(scriptableObjectClassName);

	SPDLOG_INFO("Tworzenie ScriptableObject. Id: {}, Name {}, DatPath {}", hashedName, scriptableObjectClassName, dstPath);

	ScriptableObject* createdSO = nullptr;

	if (ScriptableObject::scriptableObjects.contains(hashedName))
	{
		SPDLOG_INFO("Istnieje docelowy  ScriptableObject");
		createdSO = ScriptableObject::scriptableObjects[hashedName].createSpecificScriptableObject();
	}

	if (createdSO != nullptr)
	{
		YAML::Node node;
		YAML::Node soNode;
		createdSO->Serialize(soNode);
		node["scriptable_object"] = soNode;

		ofstream file{ dstPath };
		if (file.is_open())
		{
			file << node;
		}
		else
		{
			SPDLOG_ERROR("Couldn't open file: {}, for saving ScriptableObject!", dstPath);
		}
		file.close();
	}

	return false;
}
