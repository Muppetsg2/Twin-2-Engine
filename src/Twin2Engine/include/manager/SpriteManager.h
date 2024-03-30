#pragma once

#include <core/Sprite.h>

namespace Twin2Engine {
	namespace Manager {
		class SpriteManager {
		private:
			static std::map<size_t, Core::Sprite*> _sprites;

		public:
			static Core::Sprite* MakeSprite(Core::Texture2D* tex, const std::string& spriteAlias);
			static Core::Sprite* MakeSprite(size_t texManagerId, const std::string& spriteAlias);
			static Core::Sprite* MakeSprite(Core::Texture2D* tex, const std::string& spriteAlias, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height);
			static Core::Sprite* MakeSprite(size_t texManagerId, const std::string& spriteAlias, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height);
			
			static Core::Sprite* GetSprite(const std::string& spriteAlias);
			static Core::Sprite* GetSprite(size_t spriteId);

			static void UnloadAll();
		};
	}
}