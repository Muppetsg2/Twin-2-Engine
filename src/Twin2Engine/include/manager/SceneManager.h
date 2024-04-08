#pragma once

#include <core/Scene.h>

namespace Twin2Engine::Manager {
	class SceneManager {
	private:
		// Current Loaded Scene Data
		static Core::Scene* _currentScene;
		static Core::GameObject* _rootObject;

		// Current Loaded Resources
		static std::vector<size_t> _texturesIds;
		static std::vector<size_t> _spritesIds;
		static std::vector<size_t> _fontsIds;

		// Loaded Scene Objects
		static std::map<size_t, Core::Scene*> _loadedScenes;

		static std::pair<std::vector<size_t>, std::vector<size_t>> GetResourcesToLoadAndUnload(const std::vector<std::string> paths, const std::vector<size_t> loadedHashes);
	public:
		static void AddScene(const std::string& name, Core::Scene* scene);

		static void LoadScene(const std::string& name);

		static void UpdateCurrentScene();
		static void RenderCurrentScene();

		static void UnloadCurrent();
		static void UnloadScene(const std::string& name);
		static void UnloadAll();
	};
}