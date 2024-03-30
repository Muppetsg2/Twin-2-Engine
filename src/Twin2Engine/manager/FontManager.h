#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>
#include <core/Font.h>

namespace Twin2Engine::Manager {
	class FontManager {
	private:
		static std::map<size_t, std::map<uint32_t, Core::Font*>> _fonts;

	public:
		static void LoadFont(const std::string& fontPath, uint32_t size);
		static Core::Character* GetCharacter(const std::string& fontPath, uint32_t size, char character);
		static std::vector<Core::Character*> GetText(const std::string& fontPath, uint32_t size, const std::string& text);
	};
}