#pragma once

#include <graphic/Sprite.h>

namespace Twin2Engine::Manager {
	struct SpriteData {
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	class SpriteManager {
	private:
		static std::map<size_t, GraphicEngine::Sprite*> _sprites;

	public:
		static GraphicEngine::Sprite* MakeSprite(const std::string& spriteAlias, const std::string& texPath);
		static GraphicEngine::Sprite* MakeSprite(const std::string& spriteAlias, GraphicEngine::Texture2D* tex);
		static GraphicEngine::Sprite* MakeSprite(const std::string& spriteAlias, size_t texManagerId);
		static GraphicEngine::Sprite* MakeSprite(const std::string& spriteAlias, const std::string& texPath, const SpriteData& data);
		static GraphicEngine::Sprite* MakeSprite(const std::string& spriteAlias, GraphicEngine::Texture2D* tex, const SpriteData& data);
		static GraphicEngine::Sprite* MakeSprite(const std::string& spriteAlias, size_t texManagerId, const SpriteData& data);

		static GraphicEngine::Sprite* GetSprite(const std::string& spriteAlias);
		static GraphicEngine::Sprite* GetSprite(size_t spriteId);

		static void UnloadAll();
	};
}