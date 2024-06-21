#pragma once

#include <core/Scene.h>
#include <core/Prefab.h>

namespace Twin2Engine::Manager {
	class SceneManager {
	private:
		// Current Loaded Scene Data
		static size_t _currentSceneId;
		static std::string _currentSceneName;
		static Core::GameObject* _rootObject;

		static std::map<size_t, Core::GameObject*> _gameObjectsById;
		static std::map<size_t, Core::Component*> _componentsById;

		// Current Loaded Resources
		static std::map<size_t, size_t> _texturesIds;
		static std::map<size_t, size_t> _spritesIds;
		static std::map<size_t, size_t> _fontsIds;
		static std::map<size_t, size_t> _gifsIds;
		static std::map<size_t, size_t> _audiosIds;
		static std::map<size_t, size_t> _materialsIds;
		static std::map<size_t, size_t> _modelsIds;
		static std::map<size_t, size_t> _prefabsIds;
		static std::vector<size_t> _scriptableObjectsIds;

		// Loaded Scene Objects
		static std::map<size_t, Core::Scene*> _loadedScenes;
		static std::map<size_t, std::string> _loadedScenesNames;
		static std::map<size_t, std::string> _loadedScenesPaths;

#if _DEBUG
		// For ImGui
		static ImGuiID _selected;
		static bool _inspectorOpened;
		static const std::string _payloadType;
#endif

		static void SaveGameObject(const Core::GameObject* obj, YAML::Node gameObjects);

#if _DEBUG
		static void DrawGameObjectEditor(const Core::GameObject* obj);
#endif

		static Core::GameObject* FindObjectBy(Core::GameObject* obj, const Tools::Func<bool, const Core::GameObject*>& predicate);
		
		template<class T, class... Ts> static std::tuple<T*, Ts*...> AddComponentsToGameObject(Core::GameObject* obj)
		{
			T* comp = obj->AddComponent<T>();
			_componentsById[comp->GetId()] = comp;
			if constexpr (sizeof...(Ts) > 0) return std::tuple_cat(std::make_tuple(comp), AddComponentsToGameObject<Ts...>(obj));
			else return std::make_tuple(comp);
		};

		static Tools::EventHandler<std::string> _onSceneLoaded;

		static bool _loadScene;
		static std::string _sceneToLoadName;
		static size_t _sceneToLoadId;
		static void LoadScene();

		static std::queue<Core::GameObject*> _objectsToDestroy;
		static void DestroyObject(Core::GameObject* obj);
		static void DestroyObjects();

		static void AddComponentWithId(Core::Component* comp);
		static void RemoveComponentWithId(Core::Component* comp);

		friend class Core::Component;
	public:
		static void AddScene(const std::string& name, Core::Scene* scene);
		static void AddScene(const std::string& name, const std::string& path);

		static void LoadScene(const std::string& name);

		static void SaveScene(const std::string& path);

		static void UpdateCurrentScene();
		static void RenderCurrentScene();

		static void Update();
		static Tools::EventHandler<std::string>& GetOnSceneLoaded();
		
		static Core::GameObject* GetRootObject();
		static Core::GameObject* FindObjectByName(const std::string& name);

		template<class T>
		static typename std::enable_if_t<is_base_of_v<Core::Component, T>, Core::GameObject*> FindObjectByType() {
			for (auto& i : _componentsById) {
				if (dynamic_cast<T*>(i.second) != nullptr) {
					return i.second->GetGameObject();
				}
			}

			return nullptr;
		}

		static Core::GameObject* GetGameObjectWithId(size_t id);
		static Core::Component* GetComponentWithId(size_t id);
		template<class T> static T* GetComponentWithId(size_t id) {
			static_assert(is_base_of_v<Core::Component, T>);
			return static_cast<T*>(GetComponentWithId(id));
		};

		template<class T> 
		static typename std::enable_if_t<is_base_of_v<Core::Component, T>, std::unordered_map<size_t, Core::Component*>> GetComponentsOfType() {
			std::unordered_map<size_t, Core::Component*> items = std::unordered_map<size_t, Core::Component*>();

			for (auto& i : _componentsById) {
				if (dynamic_cast<T*>(i.second) != nullptr) {
					items[i.first] = i.second;
				}
			}

			return items;
		};

		static void DestroyGameObject(Core::GameObject* obj);

		static Core::GameObject* CreateGameObject(Core::Transform* parent = nullptr);
		template<class... Ts> static std::tuple<Core::GameObject*, Ts*...> CreateGameObject(Core::Transform* parent = nullptr) {
			Core::GameObject* obj = CreateGameObject(parent);
			return std::tuple_cat(std::make_tuple(obj), AddComponentsToGameObject<Ts...>(obj));
		}
		
		static Core::GameObject* CreateGameObject(Core::Prefab* prefab, Core::Transform* parent = nullptr);

		static size_t GetCurrentSceneId();
		static std::string GetCurrentSceneName();
		static std::string GetCurrentScenePath();

		static std::vector<std::string> GetAllLoadedScenesNames();
		static std::vector<Core::GameObject*> GetAllGameObjects();

		static size_t GetTexture2D(size_t loadIdx);
		static size_t GetSprite(size_t loadIdx);
		static size_t GetFont(size_t loadIdx);
		static size_t GetGIF(size_t loadIdx);
		static size_t GetAudio(size_t loadIdx);
		static size_t GetMaterial(size_t loadIdx);
		static size_t GetModel(size_t loadIdx);
		static size_t GetPrefab(size_t loadIdx);

		static size_t GetTexture2DSaveIdx(size_t texId);
		static size_t GetSpriteSaveIdx(size_t spriteId);
		static size_t GetFontSaveIdx(size_t fontId);
		static size_t GetGIFSaveIdx(size_t gifId);
		static size_t GetAudioSaveIdx(size_t audioId);
		static size_t GetMaterialSaveIdx(size_t materialId);
		static size_t GetModelSaveIdx(size_t modelId);
		static size_t GetPrefabSaveIdx(size_t prefabId);

		static void UnloadCurrent();
		static void UnloadScene(const std::string& name);
		static void UnloadAll();

#if _DEBUG
		static void DrawCurrentSceneEditor();
#endif
	};
}