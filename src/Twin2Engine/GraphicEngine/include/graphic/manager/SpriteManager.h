#pragma once

#include <graphic/Sprite.h>

namespace Twin2Engine::Manager {
	struct SpriteData {
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	class SceneManager;
	class PrefabManager;

	class SpriteManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, Graphic::Sprite*> _sprites;

		static std::map<size_t, std::string> _spriteAliases;

		static void UnloadSprite(size_t spriteId);
		static void UnloadSprite(const std::string& spriteAlias);

#if _DEBUG
		static bool _editNext;

		static void DrawSpriteCreator(bool* p_open);
		static void DrawSpriteEditor(bool* p_open, size_t spriteToEdit = 0);
#endif

	public:
		static Graphic::Sprite* MakeSprite(const std::string& spriteAlias, const std::string& texPath);
		static Graphic::Sprite* MakeSprite(const std::string& spriteAlias, Graphic::Texture2D* tex);
		static Graphic::Sprite* MakeSprite(const std::string& spriteAlias, size_t texManagerId);
		static Graphic::Sprite* MakeSprite(const std::string& spriteAlias, const std::string& texPath, const SpriteData& data);
		static Graphic::Sprite* MakeSprite(const std::string& spriteAlias, Graphic::Texture2D* tex, const SpriteData& data);
		static Graphic::Sprite* MakeSprite(const std::string& spriteAlias, size_t texManagerId, const SpriteData& data);

		static Graphic::Sprite* EditSprite(size_t spriteId, const std::string& texPath, const SpriteData& data, const std::string& newAlias = "");
		static Graphic::Sprite* EditSprite(size_t spriteId, Graphic::Texture2D* tex, const SpriteData& data, const std::string& newAlias = "");
		static Graphic::Sprite* EditSprite(size_t spriteId, size_t texManagerId, const SpriteData& data, const std::string& newAlias = "");
		static Graphic::Sprite* EditSprite(const std::string& spriteAlias, const std::string& texPath, const SpriteData& data, const std::string& newAlias = "");
		static Graphic::Sprite* EditSprite(const std::string& spriteAlias, Graphic::Texture2D* tex, const SpriteData& data, const std::string& newAlias = "");
		static Graphic::Sprite* EditSprite(const std::string& spriteAlias, size_t texManagerId, const SpriteData& data, const std::string& newAlias = "");

		static Graphic::Sprite* GetSprite(size_t spriteId);
		static Graphic::Sprite* GetSprite(const std::string& spriteAlias);
		
		static size_t GetSpriteId(const std::string& spriteAlias);

		static std::string GetSpriteName(size_t spriteId);
		static std::map<size_t, std::string> GetAllSpritesNames();

		static void UnloadAll();

		static YAML::Node Serialize();
#if _DEBUG
		static void DrawEditor(bool* p_open);
#endif

		friend class SceneManager;
		friend class PrefabManager;
	};
}

namespace YAML {
	template<> struct convert<Twin2Engine::Manager::SpriteData> {
		using SpriteData = Twin2Engine::Manager::SpriteData;

		static Node encode(const SpriteData& rhs) {
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["width"] = rhs.width;
			node["height"] = rhs.height;
			return node;
		}

		static bool decode(const Node& node, SpriteData& rhs) {
			if (!node.IsMap()) return false;

			rhs.x = node["x"].as<uint32_t>();
			rhs.y = node["y"].as<uint32_t>();
			rhs.width = node["width"].as<uint32_t>();
			rhs.height = node["height"].as<uint32_t>();
			return true;
		}
	};
}