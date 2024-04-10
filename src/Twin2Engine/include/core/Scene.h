#pragma once

#include <core/GameObject.h>
#include <manager/AudioManager.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/FontManager.h>

namespace Twin2Engine::Manager {
	class SceneManager;
}

namespace Twin2Engine::Core {
	class Scene {
	private:
		//GameObject* _rootObject = nullptr;
		std::vector<YAML::Node> _gameObjects = std::vector<YAML::Node>();

		std::map<std::string, Manager::TextureData> _textures = std::map<std::string, Manager::TextureData>();
		std::map<std::string, std::tuple<std::string, bool, Manager::SpriteData>> _sprites = std::map<std::string, std::tuple<std::string, bool, Manager::SpriteData>>();
		std::list<std::string> _fonts = std::list<std::string>();
		std::list<std::string> _audios = std::list<std::string>();

		//void DeleteGameObject(GameObject* obj);
		
		//template<typename T, typename... Ty> std::tuple<T*, Ty*...> AddComponentToGameObject(GameObject* obj);
	public:
		Scene() = default;
		virtual ~Scene() = default;

		// Scene Objects
		/*GameObject* AddGameObject();
		template<typename... Ty> std::tuple<GameObject*, Ty*...> AddGameObject() {
			GameObject* obj = AddGameObject();
			return std::tuple_cat(std::tuple<GameObject*>(obj), AddComponentToGameObject<Ty...>(obj));
		}*/
		void AddGameObject(const YAML::Node& gameObjectNode);

		// Scene Resources
		void AddTexture2D(const std::string& path, const Manager::TextureData& data = Manager::TextureData());
		void AddSprite(const std::string& name, const std::string& texPath);
		void AddSprite(const std::string& name, const std::string& texPath, const Manager::SpriteData& data);
		void AddFont(const std::string& path);
		void AddAudio(const std::string& path);

		friend class Manager::SceneManager;
	};

	/*template<typename T, typename... Ty>
	std::tuple<T*, Ty*...> Scene::AddComponentToGameObject(GameObject* obj) {
		T* t = obj->AddComponent<T>();
		if constexpr (sizeof...(Ty) > 0) return std::tuple_cat(std::tuple<T*>(t), AddComponentToGameObject<Ty...>(obj));
		else return std::tuple<T*>(t);
	}*/
}