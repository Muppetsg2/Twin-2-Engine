#pragma once

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
			std::map<char, Character*> _glyphs;

		private:
			Font(std::map<char, Character*> glyphs);

		public:
			virtual ~Font();

			Character* GetCharacter(char character);
			std::vector<Character*> GetText(const std::string& text);

			friend class Manager::FontManager;
		};
	}
}