#pragma once

#include <core/ScriptableObject.h>
namespace Twin2Engine::Core
{
	class ScriptableObject;
}
namespace Twin2Engine::Manager
{
	class SceneManager;
	class PrefabManager;

	class ScriptableObjectManager
	{
		friend class SceneManager;
		friend class PrefabManager;

		static std::hash<std::string> _hasher;

		// Containing of ScriptableObjects
		static std::unordered_map<size_t, Twin2Engine::Core::ScriptableObject*> _scriptableObjects;
		static std::unordered_map<size_t, std::string> _scriptableObjectsPaths;

		// Serialization of ScriptableObjects for scene
		static std::unordered_map<size_t, unsigned int> _serializationContext;
		static void SceneSerializationBegin();
		static YAML::Node Serialize();
		static void SceneSerializationEnd();

		// Deserialization of ScriptableObjects for scene
		static std::unordered_map<unsigned int, size_t> _deserializationContext;
		static void SceneDeserializationBegin();
		static size_t SceneDeserialize(unsigned int sceneSerializationId, const std::string& path);
		static void SceneDeserializationEnd();
	public: 
		static Twin2Engine::Core::ScriptableObject* Load(const std::string& path);

		static Twin2Engine::Core::ScriptableObject* Get(const std::string& path);
		static Twin2Engine::Core::ScriptableObject* Get(size_t id);
		static std::string GetPath(size_t id);

		static void UnloadAll();

		// Serialization of ScriptableObjects for scene
		static unsigned int SceneSerialize(size_t id);

		// Deserialization of ScriptableObjects for scene
		static Twin2Engine::Core::ScriptableObject* Deserialize(unsigned int sceneSerializationId);


		static std::vector<std::string> GetScriptableObjectsNames();
		static bool CreateScriptableObject(const std::string& dstPath, const std::string& scriptableObjectClassName);

		static std::vector<std::string> GetAllPaths();
	};
}