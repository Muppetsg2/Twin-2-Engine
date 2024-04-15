#pragma once

#include <core/Prefab.h>

namespace Twin2Engine::Manager {
	class SceneManager;

	class PrefabManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, Core::Prefab*> _prefabs;

		static std::map<size_t, std::string> _prefabsPaths;

		static void UnloadPrefab(size_t id);
		static void UnloadPrefab(const std::string& path);
	public:
		static Core::Prefab* LoadPrefab(const std::string& path);

		static Core::Prefab* GetPrefab(size_t id);
		static Core::Prefab* GetPrefab(const std::string& path);

		static void UnloadAll();

		friend class SceneManager;
	};
}