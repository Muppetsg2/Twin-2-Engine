#pragma once

#include <core/GameObject.h>
#include <graphic/Texture2D.h>
#include <graphic/Sprite.h>
#include <graphic/Font.h>

namespace Twin2Engine::Manager {
	class SceneManager;
}

namespace Twin2Engine::Core {
	class Scene {
	private:
		GameObject* _rootObject = nullptr;
		std::vector<std::string> _textures = std::vector<std::string>();

		void DeleteGameObject(GameObject* obj);
		
		template<typename T, typename... Ty> GameObject* AddComponentToGameObject(GameObject* obj);
	public:
		Scene();
		virtual ~Scene() = default;

		// Scene Objects
		GameObject* AddGameObject();
		template<typename... Ty> GameObject* AddGameObject() {
			GameObject* obj = AddGameObject();
			return AddComponentToGameObject<Ty...>(obj);
		}

		// Scene Resources
		void AddTexture(const std::string& path);

		friend class Manager::SceneManager;
	};

	template<typename T, typename... Ty> 
	GameObject* Scene::AddComponentToGameObject(GameObject* obj) {
		obj->AddComponent<T>();
		if constexpr (sizeof...(Ty) > 1) return AddComponentToGameObject<Ty...>(obj);
		return obj;
	}
}