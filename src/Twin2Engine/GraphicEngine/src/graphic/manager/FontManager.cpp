#include <graphic/manager/FontManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine;
using namespace Graphic;
using namespace Manager;

using namespace std;

hash<string> FontManager::_hasher;
map<size_t, Font*> FontManager::_fonts;

bool FontManager::_fileDialogOpen = false;
ImFileDialogInfo FontManager::_fileDialogInfo;

map<size_t, string> FontManager::_fontsPaths;

void FontManager::UnloadFont(size_t fontId) {
    if (_fonts.find(fontId) == _fonts.end()) return;
    delete _fonts[fontId];
    _fonts.erase(fontId);
    _fontsPaths.erase(fontId);
}

void FontManager::UnloadFont(const string& fontPath) {
    UnloadFont(hash<string>()(fontPath));
}

Font* FontManager::LoadFont(const string& fontPath) {

    if (filesystem::exists(fontPath)) {
        size_t pathHash = _hasher(fontPath);
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
        _fontsPaths[pathHash] = fontPath;
        return font;
    }
    else {
        SPDLOG_ERROR("Font file '{0}' not found!", fontPath);
        return nullptr;
    }
}

Font* FontManager::GetFont(size_t fontId) {
    if (_fonts.find(fontId) == _fonts.end()) return nullptr;
    return _fonts[fontId];
}

Font* FontManager::GetFont(const std::string& fontPath) {
    Font* font = GetFont(_hasher(fontPath));
    if (font == nullptr) {
        font = LoadFont(fontPath);
    }
    return font;
}

std::string FontManager::GetFontName(size_t fontId) {
    if (_fontsPaths.find(fontId) == _fontsPaths.end()) return "";
    string p = _fontsPaths[fontId];
    return std::filesystem::path(p).stem().string();
}

std::string FontManager::GetFontName(const std::string& fontPath) {
    return GetFontName(_hasher(fontPath));
}

std::map<size_t, std::string> FontManager::GetAllFontsNames() {
    std::map<size_t, std::string> names = std::map<size_t, std::string>();

    for (auto item : _fontsPaths) {
        names[item.first] = std::filesystem::path(item.second).stem().string();
    }
    return names;
}

void FontManager::UnloadAll() {
    for (auto& font : _fonts) {
        delete font.second;
    }
    _fonts.clear();
    _fontsPaths.clear();
}

YAML::Node FontManager::Serialize()
{
    YAML::Node fonts;
    size_t id = 0;
    for (const auto& fontPair : _fontsPaths) {
        YAML::Node font;
        font["id"] = id++;
        font["path"] = fontPair.second;
        fonts.push_back(font);
    }
    return fonts;
}

void FontManager::DrawEditor(bool* p_open) {

    if (!ImGui::Begin("Font Manager", p_open)) {
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool node_open = ImGui::TreeNodeEx(string("Fonts##Font Manager").c_str(), node_flag);

    std::list<size_t> clicked = std::list<size_t>();
    clicked.clear();
    if (node_open) {
        int i = 0;
        for (auto& item : _fontsPaths) {
            string n = GetFontName(item.second);
            ImGui::BulletText(n.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
            if (ImGui::Button(string("Remove##Font Manager").append(std::to_string(i)).c_str())) {
                clicked.push_back(item.first);
            }
            ++i;
        }
        ImGui::TreePop();
    }

    if (clicked.size() > 0) {
        clicked.sort();

        for (int i = clicked.size() - 1; i > -1; --i)
        {
            UnloadFont(clicked.back());

            clicked.pop_back();
        }
    }

    clicked.clear();

    if (ImGui::Button("Load Font##Font Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
        _fileDialogOpen = true;
        _fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
        _fileDialogInfo.title = "Open File##Font Manager";
        _fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\fonts");
    }

    if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
    {
        // Result path in: m_fileDialogInfo.resultPath
        LoadFont(_fileDialogInfo.resultPath.string());
    }

    ImGui::End();
}