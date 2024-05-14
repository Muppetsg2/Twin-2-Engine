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
	protected:
		std::vector<YAML::Node> _gameObjects = std::vector<YAML::Node>();

		std::map<size_t, std::pair<std::string, Manager::TextureData>> _textures;
		std::map<std::string, std::pair<Graphic::TextureFormat, Manager::TextureFileFormat>> _texturesFormats;
		std::map<size_t, std::tuple<std::string, size_t, bool, Manager::SpriteData>> _sprites;
		std::map<size_t, std::string> _fonts;
		std::map<size_t, std::string> _audios;
		std::map<size_t, std::string> _materials;
		std::map<size_t, std::string> _models;
		std::map<size_t, std::string> _prefabs;
		std::vector<std::string> _scriptableObjects;
	public:
		Scene() = default;
		virtual ~Scene() = default;

		void AddGameObject(const YAML::Node& gameObjectNode);

		// Scene Resources
		void AddTexture2D(size_t id, const std::string& path, const Manager::TextureData& data = Manager::TextureData());
		void AddTexture2D(size_t id, const std::string& path, const Graphic::TextureFormat& format, const Manager::TextureFileFormat& fileFormat, const Manager::TextureData& data = Manager::TextureData());
		void AddSprite(size_t id, const std::string& name, size_t textureId);
		void AddSprite(size_t id, const std::string& name, size_t textureId, const Manager::SpriteData& data);
		void AddFont(size_t id, const std::string& path);
		void AddAudio(size_t id, const std::string& path);
		void AddModel(size_t id, const std::string& path);
		void AddMaterial(size_t id, const std::string& path);
		void AddPrefab(size_t id, const std::string& path);
		void AddScriptableObject(const std::string& path);

		virtual void Deserialize(const YAML::Node& sceneNode);

		friend class Manager::SceneManager;
	};
}