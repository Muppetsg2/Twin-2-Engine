#include <graphic/manager/FontManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine;
using namespace GraphicEngine;
using namespace Manager;

using namespace std;

map<size_t, Font*> FontManager::_fonts;
hash<string> FontManager::hasher;

Font* FontManager::LoadFont(const string& fontPath) {
	size_t pathHash = hasher(fontPath);
    if (_fonts.find(pathHash) != _fonts.end()) {
        spdlog::info("Font \"{0}\" already loaded", fontPath);
        return _fonts[pathHash];
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
    Font* font = new Font(pathHash, lib, face);
    _fonts[pathHash] = font;
    return font;
}

Font* FontManager::GetFont(size_t fontId) {
    if (_fonts.find(fontId) == _fonts.end()) return nullptr;
    return _fonts[fontId];
}

Font* FontManager::GetFont(const std::string& fontPath) {
    Font* font = GetFont(hasher(fontPath));
    if (font == nullptr) {
        font = LoadFont(fontPath);
    }
    return font;
}

void FontManager::UnloadFont(size_t fontId) {
    if (_fonts.find(fontId) == _fonts.end()) return;
    delete _fonts[fontId];
    _fonts.erase(fontId);
}

void FontManager::UnloadFont(const string& fontPath) {
    UnloadFont(hash<string>()(fontPath));
}

void FontManager::UnloadAll() {
    for (auto& font : _fonts) {
        delete font.second;
    }
    _fonts.clear();
}