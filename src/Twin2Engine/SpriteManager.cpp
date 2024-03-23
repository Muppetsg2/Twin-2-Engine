#include <manager/SpriteManager.h>
#include <manager/TextureManager.h>
#include <spdlog/spdlog.h>

using namespace Twin2Engine;
using namespace Core;
using namespace Manager;
using namespace std;

map<size_t, Sprite*> SpriteManager::_sprites = map<size_t, Sprite*>();

Sprite* SpriteManager::MakeSprite(Texture2D* tex, const string& spriteAlias)
{
    return MakeSprite(tex, spriteAlias, 0, 0, tex->GetWidth(), tex->GetHeight());
}

Sprite* SpriteManager::MakeSprite(size_t texManagerId, const string& spriteAlias)
{
    return MakeSprite(TextureManager::GetTexture2D(texManagerId), spriteAlias);
}

Sprite* SpriteManager::MakeSprite(Texture2D* tex, const string& spriteAlias, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height)
{
    size_t sH = hash<string>()(spriteAlias);
    if (_sprites.find(sH) != _sprites.end()) {
        spdlog::warn("Nadpisywanie Sprite: {}", sH);
    }
    Sprite* spr = new Sprite(sH, tex, xOffset, yOffset, width, height);
    _sprites[sH] = spr;
    return spr;
}

Sprite* SpriteManager::MakeSprite(size_t texManagerId, const string& spriteAlias, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height)
{
    return MakeSprite(TextureManager::GetTexture2D(texManagerId), spriteAlias, xOffset, yOffset, width, height);
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

void SpriteManager::UnloadAll()
{
    for (auto& spr : _sprites) {
        delete spr.second;
        spr.second = nullptr;
    }
    _sprites.clear();
}