#include <Generation/MapSector.h>

using namespace Generation;
using namespace Tilemap;

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
		HexagonalTile** tileAdjacentSectors = tile->GetAdjacentTiles();


	}
}

bool MapSector::HasAdjacentSector(MapSector* otherSector) const
{

}

bool MapSector::IsInternalSector() const
{

}
