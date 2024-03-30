#include <manager/TextureManager.h>

using namespace Twin2Engine;
using namespace Core;
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

Texture2D* TextureManager::LoadTexture2D(const string& path)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        return _loadedTextures[h];
    }
    return LoadTexture2D(path, TextureWrapMode::MIRRORED_REPEAT, TextureWrapMode::MIRRORED_REPEAT, TextureFilterMode::NEAREST_MIPMAP_LINEAR, TextureFilterMode::LINEAR);
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetWrapModeS(sWrapMode);
        t->SetWrapModeT(tWrapMode);
        return t;
    }
    return LoadTexture2D(path, sWrapMode, tWrapMode, TextureFilterMode::NEAREST_MIPMAP_LINEAR, TextureFilterMode::LINEAR);
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetMinFilterMode(minFilterMode);
        t->SetMagFilterMode(magFilterMode);
        return t;
    }
    return LoadTexture2D(path, TextureWrapMode::MIRRORED_REPEAT, TextureWrapMode::MIRRORED_REPEAT, minFilterMode, magFilterMode);
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode, const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetWrapModeS(sWrapMode);
        t->SetWrapModeT(tWrapMode);
        t->SetMinFilterMode(minFilterMode);
        t->SetMagFilterMode(magFilterMode);
        return t;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);

    int width, height, channelsNr;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelsNr, 0);
    if (!data) {
        spdlog::error("Failed to load Texture: {}", path);
        stbi_image_free(data);
        return nullptr;
    }

    TextureFormat form = TextureFormat::RGB;
    if (channelsNr == 1) form = TextureFormat::RED;
    else if (channelsNr == 2) form = TextureFormat::RG;
    else if (channelsNr == 3) form = TextureFormat::RGB;
    else if (channelsNr == 4) form = TextureFormat::RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, (int)form, width, height, 0, (unsigned int)form, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    Texture2D* tex = new Texture2D(h, id, (unsigned int)width, (unsigned int)height, (unsigned int)channelsNr, form, sWrapMode, tWrapMode, minFilterMode, magFilterMode);
    _loadedTextures[h] = tex;
    return tex;
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFileFormat& internalFormat, const TextureFormat& format)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        return _loadedTextures[h];
    }
    return LoadTexture2D(path, internalFormat, format, TextureWrapMode::MIRRORED_REPEAT, TextureWrapMode::MIRRORED_REPEAT, TextureFilterMode::NEAREST_MIPMAP_LINEAR, TextureFilterMode::LINEAR);
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFileFormat& internalFormat, const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetWrapModeS(sWrapMode);
        t->SetWrapModeT(tWrapMode);
        return t;
    }
    return LoadTexture2D(path, internalFormat, format, sWrapMode, tWrapMode, TextureFilterMode::NEAREST_MIPMAP_LINEAR, TextureFilterMode::LINEAR);
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFileFormat& internalFormat, const TextureFormat& format, const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetMinFilterMode(minFilterMode);
        t->SetMagFilterMode(magFilterMode);
        return t;
    }
    return LoadTexture2D(path, internalFormat, format, TextureWrapMode::MIRRORED_REPEAT, TextureWrapMode::MIRRORED_REPEAT, minFilterMode, magFilterMode);
}

Texture2D* TextureManager::LoadTexture2D(const string& path, const TextureFileFormat& internalFormat, const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode, const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
{
    size_t h = hash<string>{}(path);
    if (_loadedTextures.find(h) != _loadedTextures.end()) {
        Texture2D* t = _loadedTextures[h];
        t->SetWrapModeS(sWrapMode);
        t->SetWrapModeT(tWrapMode);
        t->SetMinFilterMode(minFilterMode);
        t->SetMagFilterMode(magFilterMode);
        return t;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        spdlog::error("Failed to load Texture: {}", path);
        stbi_image_free(data);
        return nullptr;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    Texture2D* tex = new Texture2D(h, id, (unsigned int)width, (unsigned int)height, (unsigned int)nrChannels, format, sWrapMode, tWrapMode, minFilterMode, magFilterMode);
    _loadedTextures[h] = tex;
    return tex;
}

void TextureManager::UnloadAll()
{
    for (auto& tex : _loadedTextures) {
        delete tex.second;
        tex.second = nullptr;
    }
    _loadedTextures.clear();
}