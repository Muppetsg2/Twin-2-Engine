#include <Tilemap/HexagonalTile.h>

using namespace Tilemap;
using namespace Twin2Engine::Core;

const glm::ivec2 HexagonalTile::adjacentDirectionsEvenY[6] = {
	glm::ivec2(-1, 1), glm::ivec2(0, 1), glm::ivec2(1, 0),
	glm::ivec2(0, -1), glm::ivec2(-1, -1), glm::ivec2(-1, 0)
};

const glm::ivec2 HexagonalTile::adjacentDirectionsOddY[6] = {
	glm::ivec2(0, 1), glm::ivec2(1, 1), glm::ivec2(1, 0),
	glm::ivec2(1, -1), glm::ivec2(0, -1), glm::ivec2(-1, 0)
};

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

inline void HexagonalTile::GetAdjacentTiles(HexagonalTile** out_AdjacentTiles) const
{
	const glm::ivec2* directions = (_position.y % 2) ? adjacentDirectionsOddY : adjacentDirectionsEvenY;

	for (int i = 0; i < 6; i++)
	{
		out_AdjacentTiles[i] = _tilemap->GetTile(_position + directions[i]);
	}
}

inline void HexagonalTile::GetAdjacentGameObjects(GameObject** out_adjacentGameObjects) const
{
	const glm::ivec2* directions = (_position.y % 2) ? adjacentDirectionsOddY : adjacentDirectionsEvenY;

	for (int i = 0; i < 6; i++)
	{
		out_adjacentGameObjects[i] = _tilemap->GetTile(_position + directions[i])->GetGameObject();
	}
}