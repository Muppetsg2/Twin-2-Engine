#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <graphic/Font.h>

namespace Twin2Engine::Manager {
	class FontManager {
	private:
		static std::map<size_t, std::map<uint32_t, GraphicEngine::Font*>> _fonts;

	public:
		static void LoadFont(const std::string& fontPath, uint32_t size);
		static GraphicEngine::Character* GetCharacter(const std::string& fontPath, uint32_t size, char character);
		static std::vector<GraphicEngine::Character*> GetText(const std::string& fontPath, uint32_t size, const std::string& text);
	};
}