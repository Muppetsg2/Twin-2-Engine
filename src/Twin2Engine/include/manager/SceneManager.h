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

		static std::map<size_t, Core::GameObject*> _gameObjectsById;
		static std::map<size_t, Core::Component*> _componentsById;

		// Current Loaded Resources
		static std::vector<size_t> _texturesIds;
		static std::vector<size_t> _spritesIds;
		static std::vector<size_t> _fontsIds;
		static std::vector<size_t> _audiosIds;
		static std::vector<size_t> _materialsIds;
		static std::vector<size_t> _modelsIds;

		// Loaded Scene Objects
		static std::map<size_t, Core::Scene*> _loadedScenes;

		static std::pair<std::vector<size_t>, std::vector<size_t>> GetResourcesToLoadAndUnload(const std::vector<std::string> paths, const std::vector<size_t> loadedHashes);
		static void DeleteGameObject(Core::GameObject* obj);
		static Core::GameObject* FindObjectBy(Core::GameObject* obj, const Core::Func<bool, const Core::GameObject*>& predicate);
	public:
		static void AddScene(const std::string& name, Core::Scene* scene);
		static void AddScene(const std::string& name, const std::string& path);

		static void LoadScene(const std::string& name);

		static void UpdateCurrentScene();
		static void RenderCurrentScene();
		
		static Core::GameObject* GetRootObject();
		static Core::GameObject* FindObjectByName(const std::string& name);

		static Core::GameObject* GetGameObjectWithId(size_t id);
		static Core::Component* GetComponentWithId(size_t id);
		template<class T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true>
		static T* GetComponentWithId(size_t id) {
			return static_cast<T*>(GetComponentWithId(id));
		}

		static size_t GetTexture2D(size_t loadIdx);
		static size_t GetSprite(size_t loadIdx);
		static size_t GetFont(size_t loadIdx);
		static size_t GetAudio(size_t loadIdx);
		static size_t GetMaterial(size_t loadIdx);
		static size_t GetModel(size_t loadIdx);

		static void UnloadCurrent();
		static void UnloadScene(const std::string& name);
		static void UnloadAll();
	};
}