#pragma once

#include <core/GameObject.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/FontManager.h>

namespace Twin2Engine::Manager {
	class SceneManager;
}

namespace Twin2Engine::Core {
	class Scene {
	private:
		GameObject* _rootObject = nullptr;
		std::map<std::string, Manager::TextureData> _textures = std::map<std::string, Manager::TextureData>();

		void DeleteGameObject(GameObject* obj);
		
		template<typename T, typename... Ty> std::tuple<T*, Ty*...> AddComponentToGameObject(GameObject* obj);
	public:
		Scene();
		virtual ~Scene();

		// Scene Objects
		GameObject* AddGameObject();
		template<typename... Ty> std::tuple<GameObject*, Ty*...> AddGameObject() {
			GameObject* obj = AddGameObject();
			return std::tuple_cat(std::tuple<GameObject*>(obj), AddComponentToGameObject<Ty...>(obj));
		}

		// Scene Resources
		void AddTexture2D(const std::string& path, const Manager::TextureData& data = Manager::TextureData());

		friend class Manager::SceneManager;
	};

	template<typename T, typename... Ty> 
	std::tuple<T*, Ty*...> Scene::AddComponentToGameObject(GameObject* obj) {
		T* t = obj->AddComponent<T>();
		if constexpr (sizeof...(Ty) > 0) return std::tuple_cat(std::tuple<T*>(t), AddComponentToGameObject<Ty...>(obj));
		else return std::tuple<T*>(t);
	}
}