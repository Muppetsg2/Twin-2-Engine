#pragma once

#include <graphic/Sprite.h>

namespace Twin2Engine::Manager {
	class SpriteManager {
	private:
		static std::map<size_t, GraphicEngine::Sprite*> _sprites;

	public:
		static GraphicEngine::Sprite* MakeSprite(GraphicEngine::Texture2D* tex, const std::string& spriteAlias);
		static GraphicEngine::Sprite* MakeSprite(size_t texManagerId, const std::string& spriteAlias);
		static GraphicEngine::Sprite* MakeSprite(GraphicEngine::Texture2D* tex, const std::string& spriteAlias, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height);
		static GraphicEngine::Sprite* MakeSprite(size_t texManagerId, const std::string& spriteAlias, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height);

		static GraphicEngine::Sprite* GetSprite(const std::string& spriteAlias);
		static GraphicEngine::Sprite* GetSprite(size_t spriteId);

		static void UnloadAll();
	};
}