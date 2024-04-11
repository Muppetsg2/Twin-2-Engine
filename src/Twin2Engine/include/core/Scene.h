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
		std::vector<YAML::Node> _gameObjects = std::vector<YAML::Node>();

		std::unordered_map<std::string, Manager::TextureData> _textures = std::unordered_map<std::string, Manager::TextureData>();
		std::unordered_map<std::string, std::tuple<std::string, bool, Manager::SpriteData>> _sprites = std::unordered_map<std::string, std::tuple<std::string, bool, Manager::SpriteData>>();
		std::vector<std::string> _fonts = std::vector<std::string>();
		std::vector<std::string> _audios = std::vector<std::string>();
		std::vector<std::string> _materials = std::vector<std::string>();
		std::vector<std::tuple<bool, std::string>> _models = std::vector<std::tuple<bool, std::string>>();
	public:
		Scene() = default;
		virtual ~Scene() = default;

		void AddGameObject(const YAML::Node& gameObjectNode);

		// Scene Resources
		void AddTexture2D(const std::string& path, const Manager::TextureData& data = Manager::TextureData());
		void AddSprite(const std::string& name, const std::string& texPath);
		void AddSprite(const std::string& name, const std::string& texPath, const Manager::SpriteData& data);
		void AddFont(const std::string& path);
		void AddAudio(const std::string& path);
		void AddModel(const std::string& path, bool isStandard = false);
		void AddMaterial(const std::string& path);

		friend class Manager::SceneManager;
	};
}