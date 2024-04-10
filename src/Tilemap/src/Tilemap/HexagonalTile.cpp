#include <Tilemap/HexagonalTile.h>

using namespace Tilemap;
using namespace Twin2Engine::Core;

HexagonalTile::HexagonalTile()
{
	_gameObject = nullptr;
	_tilemap = nullptr;
	_position = glm::ivec2();
}

HexagonalTile::HexagonalTile(HexagonalTilemap* tilemap, const glm::ivec2& position)
{
	_gameObject = nullptr;
	_tilemap = tilemap;
	_position = position;
}

void HexagonalTile::SetTilemap(HexagonalTilemap* tilemap)
{
	_tilemap = tilemap;
}
inline HexagonalTilemap* HexagonalTile::GetTilemap() const
{
	return _tilemap;
}

void HexagonalTile::SetPosition(const glm::ivec2& position)
{
	_position = position;
}
inline glm::ivec2 HexagonalTile::GetPosition() const
{
	return _position;
}

void Tilemap::HexagonalTile::SetGameObject(GameObject* gameObject)
{
	_gameObject = gameObject;
}
GameObject* HexagonalTile::GetGameObject() const
{
	return _gameObject;
}
