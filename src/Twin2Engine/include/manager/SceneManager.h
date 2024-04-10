#pragma once

#include <core/Scene.h>
#include <UI/Image.h>
#include <graphic/Material.h>
#include <graphic/InstatiatingModel.h>

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
		static std::vector<size_t> _audiosIds;

		static std::vector<GraphicEngine::Material> _materialsHolder;

		static std::vector<size_t> _modelsIds;
		static std::vector<GraphicEngine::InstatiatingModel> _modelsHolder;
		static std::vector<size_t> _standardModelsIds;
		static std::vector<GraphicEngine::InstatiatingModel> _standardModelsHolder;

		// Loaded Scene Objects
		static std::map<size_t, Core::Scene*> _loadedScenes;

		static std::pair<std::vector<size_t>, std::vector<size_t>> GetResourcesToLoadAndUnload(const std::vector<std::string> paths, const std::vector<size_t> loadedHashes);
		static Core::GameObject* CreateGameObject(const YAML::Node gameObjectNode);
		static void DeleteGameObject(Core::GameObject* obj);
	public:
		static void AddScene(const std::string& name, Core::Scene* scene);
		static void AddScene(const std::string& name, const std::string& path);

		static void LoadScene(const std::string& name);

		static void UpdateCurrentScene();
		static void RenderCurrentScene();
		
		static Core::GameObject* GetRootObject();

		static void UnloadCurrent();
		static void UnloadScene(const std::string& name);
		static void UnloadAll();
	};
}