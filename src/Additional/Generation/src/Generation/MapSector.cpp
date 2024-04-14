#include <Generation/MapSector.h>

using namespace Generation;
using namespace Tilemap;

using namespace Twin2Engine::Core;

using namespace std;


void MapSector::SetTilemap(HexagonalTilemap* tilemap)
{
	_tilemap = tilemap;
}

inline HexagonalTilemap* MapSector::GetTilemap() const
{
	return _tilemap;
}

void MapSector::SetRegion(MapRegion* region)
{
	_region = region;
}

inline MapRegion* MapSector::GetRegion() const
{
	return _region;
}

void MapSector::SetType(const SectorType& type)
{
	_type = type;
}

inline MapSector::SectorType MapSector::GetType()
{
	return _type;
}


//inline const std::vector<Tilemap::HexagonalTile*>& MapSector::GetTiles() const
inline const std::unordered_set<Tilemap::HexagonalTile*>& MapSector::GetTiles() const
{
	return _sectorTiles;
}

void MapSector::AddTile(HexagonalTile* tile)
{
	_sectorTiles.insert(tile);
}

void MapSector::AddTiles(const std::vector<Tilemap::HexagonalTile*>& tiles)
{
	_sectorTiles.insert(tiles.begin(), tiles.end());
}

void MapSector::RemoveTile(Tilemap::HexagonalTile* tile)
{
	_sectorTiles.erase(tile);
}

//void MapSector::RemoveTiles(const std::vector<Tilemap::HexagonalTile*>& tiles)
void MapSector::RemoveTiles(const std::unordered_set<Tilemap::HexagonalTile*>& tiles)
{
	for (HexagonalTile* tile : tiles)
	{
		_sectorTiles.erase(tile);
	}
}


std::vector<MapSector*> MapSector::GetAdjacentSectors() const
{
	unordered_set<MapSector*> adjacentSectors;

	for (const HexagonalTile* tile : _sectorTiles)
	{
		GameObject* tileAdjacentGameObjects[6];
		tile->GetAdjacentGameObjects(tileAdjacentGameObjects);

		for (int i = 0; i < 6; i++)
		{
			if (tileAdjacentGameObjects[i] != nullptr)
			{
				MapHexTile* hexTile = tileAdjacentGameObjects[i]->GetComponent<MapHexTile>();

				if (hexTile != nullptr && hexTile->GetSector() != this)
				{
					adjacentSectors.insert(hexTile->GetSector());
				}
			}
		}
	}

	return vector<MapSector*>(adjacentSectors.cbegin(), adjacentSectors.cend());
}

bool MapSector::HasAdjacentSector(MapSector* otherSector) const
{
	for (const HexagonalTile* tile : _sectorTiles)
	{
		GameObject* tileAdjacentGameObjects[6];
		tile->GetAdjacentGameObjects(tileAdjacentGameObjects);

		for (int i = 0; i < 6; i++)
		{
			if (tileAdjacentGameObjects[i] != nullptr)
			{
				MapHexTile* hexTile = tileAdjacentGameObjects[i]->GetComponent<MapHexTile>();

				if (hexTile != nullptr && hexTile->GetSector() == otherSector)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool MapSector::IsInternalSector() const
{
	for (const HexagonalTile* tile : _sectorTiles)
	{
		GameObject* tileAdjacentGameObjects[6];
		tile->GetAdjacentGameObjects(tileAdjacentGameObjects);

		for (int i = 0; i < 6; i++)
		{
			if (tileAdjacentGameObjects[i] == nullptr)
			{
				return false;
			}
		}
	}

	return true;
}
