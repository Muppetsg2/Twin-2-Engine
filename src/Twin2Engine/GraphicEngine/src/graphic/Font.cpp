#include <graphic/Font.h>

using namespace Twin2Engine::GraphicEngine;
using namespace std;

Font::Font(size_t managerId, FT_Library lib, FT_Face face) : _managerId(managerId), _lib(lib), _face(face) {}

Font::~Font() {
	for (auto& s : _glyphs) {
		for (auto& g : s.second) {
			glDeleteTextures(1, &g.second->TextureID);
			delete g.second;
		}
		s.second.clear();
	}
	_glyphs.clear();

	FT_Done_Face(_face);
	FT_Done_FreeType(_lib);
}

void Font::LoadCharacter(char character, uint32_t size) {
    FT_Set_Pixel_Sizes(_face, 0, size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    // load character glyph 
    if (FT_Load_Char(_face, character, FT_LOAD_RENDER))
    {
        spdlog::error("ERROR::FREETYTPE: Failed to load Glyph");
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        _face->glyph->bitmap.width,
        _face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        _face->glyph->bitmap.buffer
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // now store character for later use
    Character* glyph = new Character{
        texture,
        glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
        glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
        (unsigned int)_face->glyph->advance.x
    };
    _glyphs[size].insert(std::pair<char, Character*>(character, glyph));

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

size_t Font::GetManagerId() const {
    return _managerId;
}

Character* Font::GetCharacter(char character, uint32_t size) {
    if (_glyphs.find(size) == _glyphs.end()) {
		_glyphs[size] = map<char, Character*>();
		LoadCharacter(character, size);
	}
	else if (_glyphs[size].find(character) == _glyphs[size].end()) {
		LoadCharacter(character, size);
	}
	return _glyphs[size][character];
}

vector<Character*> Font::GetText(const string& text, uint32_t size) {
	vector<Character*> res = vector<Character*>();
	for (auto& c : text) {
		res.push_back(GetCharacter(c, size));
	}
	return res;
}