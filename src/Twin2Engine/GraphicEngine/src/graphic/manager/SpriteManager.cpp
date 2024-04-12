#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine;
using namespace GraphicEngine;
using namespace Manager;
using namespace std;

map<size_t, Sprite*> SpriteManager::_sprites;
hash<string> SpriteManager::hasher;

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
    size_t aliasHash = hasher(spriteAlias);
    if (_sprites.find(aliasHash) != _sprites.end()) {
        spdlog::warn("Nadpisywanie Sprite: {}", aliasHash);
        delete _sprites[aliasHash];
    }
    Sprite* spr = new Sprite(aliasHash, tex, data.x, data.y, data.width, data.height);
    _sprites[aliasHash] = spr;
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
    return GetSprite(hasher(spriteAlias));
}

void SpriteManager::UnloadSprite(size_t spriteId) {
    if (_sprites.find(spriteId) == _sprites.end()) return;
    delete _sprites[spriteId];
    _sprites.erase(spriteId);
}

void SpriteManager::UnloadSprite(const string& spriteAlias) {
    UnloadSprite(hasher(spriteAlias));
}

void SpriteManager::UnloadAll()
{
    for (auto& spr : _sprites) {
        delete spr.second;
        spr.second = nullptr;
    }
    _sprites.clear();
}