#include <manager/FontManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine;
using namespace GraphicEngine;
using namespace Manager;

using namespace std;

map<size_t, map<uint32_t, Font*>> FontManager::_fonts = map<size_t, map<uint32_t, Font*>>();

void FontManager::LoadFont(const string& fontPath, uint32_t size) {
	size_t h = hash<string>()(fontPath);
    if (_fonts.find(h) != _fonts.end()) {
        if (_fonts[h].find(size) != _fonts[h].end()) {
            spdlog::info("Font \"{0}\" with provided size={1} already loaded", fontPath, size);
            return;
        }
    }

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        spdlog::error("ERROR::FREETYPE: Could not init FreeType Library");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        spdlog::error("ERROR::FREETYPE: Failed to load font");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    map<char, Character*> characters = map<char, Character*>();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            spdlog::error("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character* character = new Character {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        characters.insert(std::pair<char, Character*>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    _fonts[h][size] = new Font(characters);
}

Character* FontManager::GetCharacter(const string& fontPath, uint32_t size, char character)
{
    size_t h = hash<string>()(fontPath);
    if (_fonts.find(h) == _fonts.end()) {
        spdlog::error("Font \"{0}\" was not found", fontPath);
        return nullptr;
    }
    if (_fonts[h].find(size) == _fonts[h].end()) {
        LoadFont(fontPath, size);
        spdlog::warn("Size {0} for font \"{1}\" specialy loaded", size, fontPath);
    }
    return _fonts[h][size]->GetCharacter(character);
}

vector<Character*> FontManager::GetText(const string& fontPath, uint32_t size, const string& text)
{
    size_t h = hash<string>()(fontPath);
    if (_fonts.find(h) == _fonts.end()) {
        spdlog::error("Font \"{0}\" was not found", fontPath);
        return vector<Character*>();
    }
    if (_fonts[h].find(size) == _fonts[h].end()) {
        LoadFont(fontPath, size);
        spdlog::warn("Size {0} for font \"{1}\" specialy loaded", size, fontPath);
    }
    return _fonts[h][size]->GetText(text);
}