#include <graphic/manager/FontManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine;
using namespace GraphicEngine;
using namespace Manager;

using namespace std;

map<size_t, Font*> FontManager::_fonts = map<size_t, Font*>();

Font* FontManager::LoadFont(const string& fontPath) {
	size_t h = hash<string>()(fontPath);
    if (_fonts.find(h) != _fonts.end()) {
        spdlog::info("Font \"{0}\" already loaded", fontPath);
        return _fonts[h];
    }

    FT_Library lib;
    if (FT_Init_FreeType(&lib)) {
        spdlog::error("ERROR::FREETYPE: Could not init FreeType Library");
        return nullptr;
    }

    FT_Face face;
    if (FT_New_Face(lib, fontPath.c_str(), 0, &face)) {
        spdlog::error("ERROR::FREETYPE: Failed to load font");
        return nullptr;
    }
    Font* font = new Font(lib, face);
    _fonts[h] = font;
    return font;
}

void FontManager::UnloadAll() {
    for (auto& font : _fonts) {
        delete font.second;
    }
    _fonts.clear();
}