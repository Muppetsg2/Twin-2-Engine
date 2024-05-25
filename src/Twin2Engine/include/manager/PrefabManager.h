#pragma once

#include <core/Prefab.h>

namespace Twin2Engine::Manager {
	class SceneManager;

	class PrefabManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, Core::Prefab*> _prefabs;

#if _DEBUG
		// For ImGui
		static bool _fileDialogOpen;
		static ImFileDialogInfo _fileDialogInfo;
#endif

		static std::map<size_t, std::string> _prefabsPaths;

		static void SaveGameObject(const Core::GameObject* obj, YAML::Node gameObjects);

		static void UnloadPrefab(size_t id);
		static void UnloadPrefab(const std::string& path);
	public:
		static Core::Prefab* LoadPrefab(const std::string& path);

		static Core::Prefab* GetPrefab(size_t id);
		static Core::Prefab* GetPrefab(const std::string& path);
		static std::string GetPrefabName(size_t id);
		static std::string GetPrefabName(const std::string& path);
		static std::string GetPrefabPath(const Core::Prefab* prefab);

		static std::map<size_t, string> GetAllPrefabsNames();

		static void SaveAsPrefab(const Core::GameObject* obj, const std::string& path);

		static void UnloadAll();

		static YAML::Node Serialize();
		
#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif

		friend class SceneManager;
	};
}