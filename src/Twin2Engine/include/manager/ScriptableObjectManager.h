#pragma once

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

#if _DEBUG
		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;
#endif

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

#if _DEBUG
		static void DrawCreator(bool* p_open);
#endif

	public: 
		static Twin2Engine::Core::ScriptableObject* Load(const std::string& path);
		static void Unload(const std::string& path);
		static void Unload(size_t id);

		static Twin2Engine::Core::ScriptableObject* Get(const std::string& path);
		static Twin2Engine::Core::ScriptableObject* Get(size_t id);
		static std::string GetPath(size_t id);
		static std::string GetName(const std::string& path);
		static std::string GetName(size_t id);

		static void UnloadAll();

		// Serialization of ScriptableObjects for scene
		static unsigned int SceneSerialize(size_t id);

		// Deserialization of ScriptableObjects for scene
		static Twin2Engine::Core::ScriptableObject* Deserialize(unsigned int sceneSerializationId);

#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif

		template<class T>
		static typename std::enable_if_t<std::is_base_of_v<Twin2Engine::Core::ScriptableObject, T>, std::map<size_t, Twin2Engine::Core::ScriptableObject*>> GetScriptableObjectsDerivedByType() {
			
			std::map<size_t, Twin2Engine::Core::ScriptableObject*> ret = std::map<size_t, Twin2Engine::Core::ScriptableObject*>();

			for (auto& item : _scriptableObjects) {
				
				if (dynamic_cast<T*>(item.second) != nullptr) {
					ret[item.first] = item.second;
				}
			}

			return ret;
		}

		static std::vector<std::string> GetScriptableObjectsNames();
		static std::vector<std::string> GetScriptableObjectsClassNames();
		static bool CreateScriptableObject(const std::string& dstPath, const std::string& scriptableObjectClassName);
		static bool Save(const std::string& dstPath, Twin2Engine::Core::ScriptableObject* scriptableObject);

		static std::vector<std::string> GetAllPaths();
	};
}