#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <graphic/Texture2D.h>

namespace Twin2Engine {

	namespace Manager {
		class FontManager;
	}

	namespace Graphic {
		struct Character {
			Texture2D* texture;  // ID handle of the glyph texture
			glm::ivec2   Size;       // Size of glyph
			glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
			unsigned int Advance;    // Offset to advance to next glyph
		};

		class Font {
		private:
			size_t _managerId;
			std::map<uint32_t, std::map<unsigned int, Character*>> _glyphs;
			FT_Library _lib;
			FT_Face _face;

		private:
			Font(size_t managerId, FT_Library lib, FT_Face face);

			void LoadCharacter(unsigned int character, uint32_t size);

		public:
			virtual ~Font();

			size_t GetManagerId() const;
			Character* GetCharacter(unsigned int character, uint32_t size);

			friend class Manager::FontManager;
		};
	}
}