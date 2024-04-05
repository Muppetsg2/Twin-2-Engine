#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine;
using namespace GraphicEngine;
using namespace Manager;
using namespace std;

map<size_t, Texture2D*> TextureManager::_loadedTextures = map<size_t, Texture2D*>();

Texture2D* TextureManager::GetTexture2D(size_t managerId)
{
    if (_loadedTextures.find(managerId) != _loadedTextures.end()) {
        return _loadedTextures[managerId];
    }
    return nullptr;
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureData& data)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetWrapModeS(data.sWrapMode);
        t->SetWrapModeT(data.tWrapMode);
        t->SetMinFilterMode(data.minFilterMode);
        t->SetMagFilterMode(data.magFilterMode);
        return t;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data.sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data.tWrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data.magFilterMode);

    int width, height, channelsNr;
    unsigned char* imgData = stbi_load(path.c_str(), &width, &height, &channelsNr, 0);
    if (!imgData) {
        spdlog::error("Failed to load Texture: {}", path);
        stbi_image_free(imgData);
        return nullptr;
    }

    TextureFormat form = TextureFormat::RGB;
    if (channelsNr == 1) form = TextureFormat::RED;
    else if (channelsNr == 2) form = TextureFormat::RG;
    else if (channelsNr == 3) form = TextureFormat::RGB;
    else if (channelsNr == 4) form = TextureFormat::RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, (int)form, width, height, 0, (unsigned int)form, GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imgData);

    Texture2D* tex = new Texture2D(h, id, (unsigned int)width, (unsigned int)height, (unsigned int)channelsNr, form, data.sWrapMode, data.tWrapMode, data.minFilterMode, data.magFilterMode);
    _loadedTextures[h] = tex;
    return tex;
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFileFormat& internalFormat, const TextureFormat& format, const TextureData& data)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetWrapModeS(data.sWrapMode);
        t->SetWrapModeT(data.tWrapMode);
        t->SetMinFilterMode(data.minFilterMode);
        t->SetMagFilterMode(data.magFilterMode);
        return t;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data.sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data.tWrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data.magFilterMode);

    int width, height, nrChannels;
    unsigned char* imgData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!imgData)
    {
        spdlog::error("Failed to load Texture: {}", path);
        stbi_image_free(imgData);
        return nullptr;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imgData);

    Texture2D* tex = new Texture2D(h, id, (unsigned int)width, (unsigned int)height, (unsigned int)nrChannels, format, data.sWrapMode, data.tWrapMode, data.minFilterMode, data.magFilterMode);
    _loadedTextures[h] = tex;
    return tex;
}

void TextureManager::UnloadTexture2D(size_t managerID)
{
    if (_loadedTextures.find(managerID) == _loadedTextures.end()) return;
    delete _loadedTextures[managerID];
    _loadedTextures.erase(managerID);
}

void TextureManager::UnloadTexture2D(const string& path)
{
    UnloadTexture2D(hash<string>()(path));
}

void TextureManager::UnloadAll()
{
    for (auto& tex : _loadedTextures) {
        delete tex.second;
        tex.second = nullptr;
    }
    _loadedTextures.clear();
}