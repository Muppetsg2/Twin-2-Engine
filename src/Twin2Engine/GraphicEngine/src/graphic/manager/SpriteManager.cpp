#include <graphic/manager/SpriteManager.h>
#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine;
using namespace GraphicEngine;
using namespace Manager;
using namespace std;

map<size_t, Sprite*> SpriteManager::_sprites = map<size_t, Sprite*>();

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
    size_t sH = hash<string>()(spriteAlias);
    if (_sprites.find(sH) != _sprites.end()) {
        spdlog::warn("Nadpisywanie Sprite: {}", sH);
        delete _sprites[sH];
    }
    Sprite* spr = new Sprite(sH, tex, data.x, data.y, data.width, data.height);
    _sprites[sH] = spr;
    return spr;
}

Sprite* SpriteManager::MakeSprite(const string& spriteAlias, size_t texManagerId, const SpriteData& data)
{
    return MakeSprite(spriteAlias, TextureManager::GetTexture2D(texManagerId), data);
}

Sprite* SpriteManager::GetSprite(const string& spriteAlias)
{
    size_t sH = hash<string>()(spriteAlias);
    return GetSprite(sH);
}

Sprite* SpriteManager::GetSprite(size_t spriteId)
{
    if (_sprites.find(spriteId) != _sprites.end()) {
        return _sprites[spriteId];
    }
    return nullptr;
}

void SpriteManager::UnloadSprite(size_t spriteId) {
    if (_sprites.find(spriteId) == _sprites.end()) return;
    delete _sprites[spriteId];
    _sprites.erase(spriteId);
}

void SpriteManager::UnloadSprite(const string& spriteAlias) {
    UnloadSprite(hash<string>()(spriteAlias));
}

void SpriteManager::UnloadAll()
{
    for (auto& spr : _sprites) {
        delete spr.second;
        spr.second = nullptr;
    }
    _sprites.clear();
}