#pragma once

#include <core/Scene.h>

namespace Twin2Engine::Manager {
	class SceneManager {
	private:
		// Current Loaded Scene Data
		static Core::Scene* _currentScene;
		static std::vector<size_t> _texturesIds; // na razie tylko tekstury
		static Core::GameObject* _rootObject;

		// Loaded Scene Objects
		static std::map<size_t, Core::Scene*> _loadedScenes;
	public:
		static void AddScene(const std::string& name, Core::Scene* scene);

		static void LoadScene(const std::string& name);

		static void UpdateCurrentScene();
		static void RenderCurrentScene();
	};
}