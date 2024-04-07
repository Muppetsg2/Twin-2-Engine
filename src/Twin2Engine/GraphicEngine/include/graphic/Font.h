#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Twin2Engine {

	namespace Manager {
		class FontManager;
	}

	namespace GraphicEngine {
		struct Character {
			unsigned int TextureID;  // ID handle of the glyph texture
			glm::ivec2   Size;       // Size of glyph
			glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
			unsigned int Advance;    // Offset to advance to next glyph
		};

		class Font {
		private:
			std::map<uint32_t, std::map<char, Character*>> _glyphs = std::map<uint32_t, std::map<char, Character*>>();
			FT_Library _lib;
			FT_Face _face;

		private:
			Font(FT_Library lib, FT_Face face);

			void LoadCharacter(char character, uint32_t size);

		public:
			virtual ~Font();

			Character* GetCharacter(char character, uint32_t size);
			std::vector<Character*> GetText(const std::string& text, uint32_t size);

			friend class Manager::FontManager;
		};
	}
}