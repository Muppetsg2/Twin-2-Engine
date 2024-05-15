#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine;
using namespace Graphic;
using namespace Manager;
using namespace std;

hash<string> SpriteManager::_hasher;
map<size_t, Sprite*> SpriteManager::_sprites;

map<size_t, string> SpriteManager::_spriteAliases;
map<size_t, SpriteData> SpriteManager::_spriteLoadData;

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, const string& texPath)
{
    return MakeSprite(spriteAlias, TextureManager::LoadTexture2D(texPath));
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, Texture2D* tex)
{
    return MakeSprite(spriteAlias, tex, { .x = 0, .y = 0, .width = tex->GetWidth(), .height = tex->GetHeight() });
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, size_t texManagerId)
{
    return MakeSprite(spriteAlias, TextureManager::GetTexture2D(texManagerId));
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, const string& texPath, const SpriteData& data)
{
    return MakeSprite(spriteAlias, TextureManager::LoadTexture2D(texPath), data);
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, Texture2D* tex, const SpriteData& data)
{
    size_t aliasHash = _hasher(spriteAlias);
    if (_sprites.find(aliasHash) != _sprites.end()) {
        spdlog::warn("Sprite o aliasie '{0}' ju¿ istnieje", aliasHash);
        return _sprites[aliasHash];
    }
    Sprite* spr = new Sprite(aliasHash, tex, data.x, data.y, data.width, data.height);
    _sprites[aliasHash] = spr;
    _spriteAliases[aliasHash] = spriteAlias;

    if (data.x != 0 || data.y != 0 || data.width != tex->GetWidth() || data.height != tex->GetHeight()) {
        _spriteLoadData[aliasHash] = data;
    }

    return spr;
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, size_t texManagerId, const SpriteData& data)
{
    return MakeSprite(spriteAlias, TextureManager::GetTexture2D(texManagerId), data);
}

Sprite* SpriteManager::GetSprite(size_t spriteId)
{
    if (_sprites.find(spriteId) != _sprites.end()) {
        return _sprites[spriteId];
    }
    return nullptr;
}

Sprite* SpriteManager::GetSprite(const string& spriteAlias)
{
    return GetSprite(_hasher(spriteAlias));
}

std::map<size_t, std::string> SpriteManager::GetAllSpritesNames() {
    std::map<size_t, std::string> names = std::map<size_t, std::string>();

    for (auto item : _spriteAliases) {
        names[item.first] = item.second;
    }
    return names;
}

void SpriteManager::UnloadSprite(size_t spriteId) {
    if (_sprites.find(spriteId) == _sprites.end()) return;
    delete _sprites[spriteId];
    _sprites.erase(spriteId);
    _spriteAliases.erase(spriteId);
}

void SpriteManager::UnloadSprite(const string& spriteAlias) {
    UnloadSprite(_hasher(spriteAlias));
}

void SpriteManager::UnloadAll()
{
    for (auto& spr : _sprites) {
        delete spr.second;
    }
    _sprites.clear();
    _spriteAliases.clear();
}

YAML::Node SpriteManager::Serialize()
{
    YAML::Node sprites;
    size_t id = 0;
    for (const auto& spritePair : SpriteManager::_spriteAliases) {
        Sprite* sprite = SpriteManager::_sprites[spritePair.first];

        YAML::Node spriteNode;
        spriteNode["id"] = id++;
        spriteNode["alias"] = spritePair.second;
        spriteNode["texture"] = sprite->GetTexture()->GetManagerId();

        if (SpriteManager::_spriteLoadData.contains(spritePair.first)) {
            SpriteData data = SpriteManager::_spriteLoadData[spritePair.first];
            spriteNode["x"] = data.x;
            spriteNode["y"] = data.y;
            spriteNode["width"] = data.width;
            spriteNode["height"] = data.height;
        }

        sprites.push_back(spriteNode);
    }
    return sprites;
}
