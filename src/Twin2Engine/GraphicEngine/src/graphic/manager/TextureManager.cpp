#include <graphic/manager/TextureManager.h>

#if _DEBUG
#include <regex>
#endif

using namespace Twin2Engine;
using namespace Graphic;
using namespace Manager;
using namespace std;

hash<string> TextureManager::_hasher;
map<size_t, Texture2D*> TextureManager::_loadedTextures;

map<size_t, string> TextureManager::_texturesPaths;
map<size_t, pair<TextureFormat, TextureFileFormat>> TextureManager::_texturesFormats;

#if _DEBUG
// For ImGui
bool TextureManager::_fileDialogOpen = false;
ImFileDialogInfo TextureManager::_fileDialogInfo;
#endif

void TextureManager::UnloadTexture2D(size_t managerID)
{
    if (!_loadedTextures.contains(managerID)) return;
    delete _loadedTextures[managerID];
    _loadedTextures.erase(managerID);
    _texturesPaths.erase(managerID);
    _texturesFormats.erase(managerID);
}

void TextureManager::UnloadTexture2D(const string& path)
{
    UnloadTexture2D(_hasher(path));
}

bool TextureManager::IsTextureLoaded(size_t managerId) {
    return _loadedTextures.contains(managerId);
}

bool TextureManager::IsTextureLoaded(const std::string& path) {
    return _loadedTextures.contains(_hasher(path));
}

Texture2D* TextureManager::FindTextureWithProgramID(GLuint programId) {
    auto iter = std::find_if(_loadedTextures.begin(), _loadedTextures.end(), [&](std::pair<size_t, Texture2D*> tex) -> bool {
        return tex.second->GetId() == programId;
    });

    if (iter == _loadedTextures.end()) return nullptr;

    return iter->second;
}

Texture2D* TextureManager::GetTexture2D(size_t managerId)
{
    if (_loadedTextures.contains(managerId)) {
        return _loadedTextures[managerId];
    }
    return nullptr;
}

Texture2D* TextureManager::GetTexture2D(const std::string& path)
{
    Texture2D* tex = GetTexture2D(_hasher(path));
    if (tex == nullptr) {
        tex = LoadTexture2D(path);
    }
    return tex;
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureData& data)
{
    size_t pathHash = _hasher(path);
    if (_loadedTextures.contains(pathHash)) {
        return _loadedTextures[pathHash];
    }

    unsigned int id;
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)data.sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)data.tWrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)data.minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)data.magFilterMode);

    int width, height, channelsNr;
    unsigned char* imgData = stbi_load(path.c_str(), &width, &height, &channelsNr, 0);
    if (!imgData) {
        spdlog::error("Failed to load Texture: {}", path);
        stbi_image_free(imgData);
        return nullptr;
    }

    TextureFormat form = TextureFormat::RGB;
    TextureFileFormat inter = TextureFileFormat::SRGB;
    if (channelsNr == 1) { form = TextureFormat::RED; inter = TextureFileFormat::RED; }
    else if (channelsNr == 2) { form = TextureFormat::RG; inter = TextureFileFormat::RG; }
    else if (channelsNr == 3) { form = TextureFormat::RGB; inter = TextureFileFormat::SRGB; }
    else if (channelsNr == 4) { form = TextureFormat::RGBA; inter = TextureFileFormat::SRGBA; }

    glTexImage2D(GL_TEXTURE_2D, 0, (unsigned int)inter, width, height, 0, (unsigned int)form, GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imgData);

    Texture2D* tex = new Texture2D(pathHash, id, (unsigned int)width, (unsigned int)height, (unsigned int)channelsNr, form, data.sWrapMode, data.tWrapMode, data.minFilterMode, data.magFilterMode);
    _loadedTextures[pathHash] = tex;
    _texturesPaths[pathHash] = path;
    //_texturesFormats[pathHash] = { form, (TextureFileFormat)inter };
    return tex;
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFileFormat& internalFormat, const TextureFormat& format, const TextureData& data)
{
    size_t pathHash = _hasher(path);
    if (_loadedTextures.find(pathHash) != _loadedTextures.end()) {
        return _loadedTextures[pathHash];
    }

    unsigned int id;
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)data.sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)data.tWrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)data.minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)data.magFilterMode);

    int width, height, nrChannels;
    unsigned char* imgData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!imgData)
    {
        spdlog::error("Failed to load Texture: {}", path);
        stbi_image_free(imgData);
        return nullptr;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, (GLint)internalFormat, width, height, 0, (GLint)format, GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imgData);

    Texture2D* tex = new Texture2D(pathHash, id, (unsigned int)width, (unsigned int)height, (unsigned int)nrChannels, format, data.sWrapMode, data.tWrapMode, data.minFilterMode, data.magFilterMode);
    _loadedTextures[pathHash] = tex;
    _texturesPaths[pathHash] = path;
    _texturesFormats[pathHash] = { format, internalFormat };
    return tex;
}

std::string TextureManager::GetTexture2DName(size_t managerId) {
    if (!_texturesPaths.contains(managerId)) return "";
    string p = _texturesPaths[managerId];
    return std::filesystem::path(p).stem().string();
}

std::string TextureManager::GetTexture2DName(const std::string& path) {
    if (!_texturesPaths.contains(_hasher(path))) return "";
    return std::filesystem::path(path).stem().string();
}

#if _DEBUG
std::string TextureManager::GetTexture2DPath(size_t managerId) {
    if (!_texturesPaths.contains(managerId)) return "";
    return _texturesPaths[managerId];
}
#endif

std::map<size_t, std::string> TextureManager::GetAllTexture2DNames() {
    std::map<size_t, std::string> names = std::map<size_t, std::string>();

    for (auto item : _texturesPaths) {
        names[item.first] = std::filesystem::path(item.second).stem().string();
    }
    return names;
}

void TextureManager::UnloadAll()
{
    for (auto& tex : _loadedTextures) {
        delete tex.second;
    }
    _loadedTextures.clear();
    _texturesPaths.clear();
    _texturesFormats.clear();
}

YAML::Node TextureManager::Serialize()
{
    YAML::Node textures;
    size_t id = 0;
    for (const auto& pathPair : _texturesPaths) {
        Texture2D* tex = _loadedTextures[pathPair.first];

        YAML::Node texNode;
        texNode["id"] = id++;
        texNode["path"] = pathPair.second;
        if (_texturesFormats.contains(pathPair.first)) {
            const auto& formats = _texturesFormats[pathPair.first];
            texNode["fileFormat"] = formats.second;
            texNode["engineFormat"] = formats.first;
        }
        texNode["sWrapMode"] = tex->GetWrapModeS();
        texNode["tWrapMode"] = tex->GetWrapModeT();
        texNode["minFilterMode"] = tex->GetMinFilterMode();
        texNode["magFilterMode"] = tex->GetMagFilterMode();

        textures.push_back(texNode);
    }
    return textures;
}

#if _DEBUG
void TextureManager::DrawEditor(bool* p_open)
{
    if (!ImGui::Begin("Texture Manager", p_open)) {
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool node_open = ImGui::TreeNodeEx(string("Textures##Texture Manager").c_str(), node_flag);

    std::list<size_t> clicked = std::list<size_t>();
    static size_t selectedToEdit = 0;
    static bool openEditor = true;
    clicked.clear();
    if (node_open) {
        int i = 0;
        for (auto& item : _texturesPaths) {
            string n = GetTexture2DName(item.second);
            ImGui::BulletText(n.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 35);
            if (ImGui::Button(string(ICON_FA_PENCIL "##Edit Texture Manager").append(std::to_string(i)).c_str())) {
                selectedToEdit = item.first;
                openEditor = true;
            }
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
            if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove Texture Manager").append(std::to_string(i)).c_str())) {
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
            UnloadTexture2D(clicked.back());

            clicked.pop_back();
        }
    }
    clicked.clear();

    if (selectedToEdit != 0) {
        if (ImGui::Begin("Texture Editor##Texture Manager", &openEditor)) {
            _loadedTextures[selectedToEdit]->DrawEditor();
        }
        ImGui::End();

        if (!openEditor) {
            selectedToEdit = 0;
        }
    }

    if (ImGui::Button("Load Texture##Texture Manager", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
        _fileDialogOpen = true;
        _fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
        _fileDialogInfo.title = "Open File##Texture Manager";
        _fileDialogInfo.directoryPath = std::filesystem::path(std::filesystem::current_path().string() + "\\res\\textures");
    }

    if (ImGui::FileDialog(&_fileDialogOpen, &_fileDialogInfo))
    {
        ImGui::OpenPopup(string("Load Parameters PopUp##Texture Manager").c_str(), ImGuiPopupFlags_NoReopen);
        // Result path in: m_fileDialogInfo.resultPath
        //LoadTexture2D(_fileDialogInfo.resultPath.string());
    }

    if (ImGui::BeginPopup(string("Load Parameters PopUp##Texture Manager").c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

        const std::vector<TextureFormat> formats {
            TextureFormat::RED,
            TextureFormat::RG,
            TextureFormat::RGB,
            TextureFormat::BGR,
            TextureFormat::RGBA,
            TextureFormat::BGRA,
            TextureFormat::R_INT,
            TextureFormat::RG_INT,
            TextureFormat::RGB_INT,
            TextureFormat::BGR_INT,
            TextureFormat::RGBA_INT,
            TextureFormat::BGRA_INT,
            TextureFormat::STENCIL_IDX,
            TextureFormat::DEPTH_COMPONENT,
            TextureFormat::DEPTH_STENCIL
        };

        const std::vector<TextureFileFormat> interFormats{
            TextureFileFormat::DEPTH_COMPONENT,
            TextureFileFormat::DEPTH_STENCIL,
            TextureFileFormat::RED,
            TextureFileFormat::RG,
            TextureFileFormat::RGB,
            TextureFileFormat::SRGB,
            TextureFileFormat::RGBA,
            TextureFileFormat::SRGBA,
            TextureFileFormat::R8,
            TextureFileFormat::R8_SNORM,
            TextureFileFormat::R16,
            TextureFileFormat::R16_SNORM,
            TextureFileFormat::RG8,
            TextureFileFormat::RG8_SNORM,
            TextureFileFormat::RG16,
            TextureFileFormat::RG16_SNORM,
            TextureFileFormat::RG3_B2,
            TextureFileFormat::RGB4,
            TextureFileFormat::RGB5,
            TextureFileFormat::RGB8,
            TextureFileFormat::RGB8_SNORM,
            TextureFileFormat::SRGB8,
            TextureFileFormat::RGB10,
            TextureFileFormat::RGB12,
            TextureFileFormat::RGB16_SNORM,
            TextureFileFormat::RGBA2,
            TextureFileFormat::RGBA4,
            TextureFileFormat::RGB5_A1,
            TextureFileFormat::RGBA8,
            TextureFileFormat::RGBA8_SNORM,
            TextureFileFormat::SRGBA8,
            TextureFileFormat::RGB10_A2,
            TextureFileFormat::RGB10_A2_UINT,
            TextureFileFormat::RGBA12,
            TextureFileFormat::RGBA16,
            TextureFileFormat::R16_FLOAT,
            TextureFileFormat::RG16_FLOAT,
            TextureFileFormat::RGB16_FLOAT,
            TextureFileFormat::RGBA16_FLOAT,
            TextureFileFormat::R32_FLOAT,
            TextureFileFormat::RG32_FLOAT,
            TextureFileFormat::RGB32_FLOAT,
            TextureFileFormat::RGBA32_FLOAT,
            TextureFileFormat::RG11_B10_FLOAT,
            TextureFileFormat::RGB9_E5,
            TextureFileFormat::R8_INT,
            TextureFileFormat::R8_UINT,
            TextureFileFormat::R16_INT,
            TextureFileFormat::R16_UINT,
            TextureFileFormat::R32_INT,
            TextureFileFormat::R32_UINT,
            TextureFileFormat::RG8_INT,
            TextureFileFormat::RG8_UINT,
            TextureFileFormat::RG16_INT,
            TextureFileFormat::RG16_UINT,
            TextureFileFormat::RG32_INT,
            TextureFileFormat::RG32_UINT,
            TextureFileFormat::RGB8_INT,
            TextureFileFormat::RGB8_UINT,
            TextureFileFormat::RGB16_INT,
            TextureFileFormat::RGB16_UINT,
            TextureFileFormat::RGB32_INT,
            TextureFileFormat::RGB32_UINT,
            TextureFileFormat::RGBA8_INT,
            TextureFileFormat::RGBA8_UINT,
            TextureFileFormat::RGBA16_INT,
            TextureFileFormat::RGBA16_UINT,
            TextureFileFormat::RGBA32_INT,
            TextureFileFormat::RGBA32_UINT,
            TextureFileFormat::COMPRESSED_R,
            TextureFileFormat::COMPRESSED_RG,
            TextureFileFormat::COMPRESSED_RGB,
            TextureFileFormat::COMPRESSED_RGBA,
            TextureFileFormat::COMPRESSED_SRGB,
            TextureFileFormat::COMPRESSED_SRGBA,
            TextureFileFormat::COMPRESSED_R_RGTC1,
            TextureFileFormat::COMPRESSED_SIGNED_R_RGTC1,
            TextureFileFormat::COMPRESSED_RG_RGTC2,
            TextureFileFormat::COMPRESSED_SIGNED_RG_RGTC2,
            TextureFileFormat::COMPRESSED_RGBA_BPTC_UNORM,
            TextureFileFormat::COMPRESSED_SRGBA_BPTC_UNORM,
            TextureFileFormat::COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
            TextureFileFormat::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
        };

        static bool detect = false;

        ImGui::Checkbox("Detect##Texture Manager PopUp", &detect);

        static TextureFileFormat inter = TextureFileFormat::SRGBA;
        static TextureFormat form = TextureFormat::RGBA;
        bool clicked = false;

        ImGui::BeginDisabled(detect);
        if (ImGui::BeginCombo(std::string("Internal Format##Texture Manager PopUp").c_str(), to_string(inter).c_str())) {

            for (auto item : interFormats)
            {
                if (ImGui::Selectable(to_string(item).append("##Internal Format Texture Manager PopUp").c_str(), inter == item))
                {
                    if (clicked) {
                        continue;
                    }
                    inter = item;
                    clicked = true;
                }
            }
            ImGui::EndCombo();
        }

        clicked = false;
        if (ImGui::BeginCombo(std::string("Format##Texture Manager PopUp").c_str(), to_string(form).c_str())) {

            for (auto item : formats)
            {
                if (ImGui::Selectable(to_string(item).append("##Format Texture Manager PopUp").c_str(), form == item))
                {
                    if (clicked) {
                        continue;
                    }
                    form = item;
                    clicked = true;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();

        if (ImGui::Button("Load##Texture Manager PopUp", ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {

            string path = std::filesystem::relative(_fileDialogInfo.resultPath).string();

            if (std::regex_search(path, std::regex("(?:[/\\\\]res[/\\\\])"))) {
                path = path.substr(path.find("res"));
            }

            if (detect) LoadTexture2D(path);
            else LoadTexture2D(path, inter, form);

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}
#endif