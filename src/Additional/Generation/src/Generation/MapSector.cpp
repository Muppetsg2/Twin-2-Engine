#include <Generation/MapSector.h>

#include <Generation/YamlConverters.h>


using namespace Generation;
using namespace Tilemap;

using namespace Twin2Engine::Core;

using namespace std;


//void MapSector::SetTilemap(MapHexTilemap* tilemap)
//{
//	_tilemap = tilemap;
//}
//
//inline HexagonalTilemap* MapSector::GetTilemap() const
//{
//	return _tilemap;
//}

//void MapSector::SetRegion(MapRegion* region)
//{
//	_region = region;
//}
//
//inline MapRegion* MapSector::GetRegion() const
//{
//	return _region;
//}

//void MapSector::SetType(const SectorType& type)
//{
//	_type = type;
//}
//
//inline MapSector::SectorType MapSector::GetType()
//{
//	return _type;
//}


//inline const std::vector<Tilemap::MapHexTile*>& MapSector::GetTiles() const
const std::unordered_set<MapHexTile*>& MapSector::GetTiles() const
{
	return _sectorTiles;
}

void MapSector::AddTile(MapHexTile* tile)
{
	_sectorTiles.insert(tile);
	tile->sector = this;
	tile->GetTransform()->SetParent(GetTransform());
}

void MapSector::AddTiles(const std::vector<MapHexTile*>& tiles)
{
	_sectorTiles.insert(tiles.begin(), tiles.end());
	for (MapHexTile* tile : tiles)
	{
		tile->sector = this;
	}
}

void MapSector::JoinSector(MapSector* otherSector)
{
	_sectorTiles.insert(otherSector->_sectorTiles.begin(), otherSector->_sectorTiles.end());
	for (const auto& tile : otherSector->_sectorTiles)
	{
		tile->GetTransform()->SetParent(GetTransform());
		tile->sector = this;
	}
	otherSector->_sectorTiles.clear();
}

void MapSector::RemoveTile(MapHexTile* tile)
{
	_sectorTiles.erase(tile);
}

//void MapSector::RemoveTiles(const std::vector<MapHexTile*>& tiles)
void MapSector::RemoveTiles(const std::unordered_set<MapHexTile*>& tiles)
{
	for (MapHexTile* tile : tiles)
	{
		_sectorTiles.erase(tile);
	}
}

size_t MapSector::GetTilesCount() const
{
	return _sectorTiles.size();
}

std::vector<MapSector*> MapSector::GetAdjacentSectors() const
{
	unordered_set<MapSector*> adjacentSectors;

	for (const MapHexTile* tile : _sectorTiles)
	{
		GameObject* tileAdjacentGameObjects[6];
		tile->tile->GetAdjacentGameObjects(tileAdjacentGameObjects);

		for (int i = 0; i < 6; i++)
		{
			if (tileAdjacentGameObjects[i] != nullptr)
			{
				MapHexTile* hexTile = tileAdjacentGameObjects[i]->GetComponent<MapHexTile>();

				if (hexTile != nullptr && hexTile->sector != this)
				{
					adjacentSectors.insert(hexTile->sector);
				}
			}
		}
	}

	return vector<MapSector*>(adjacentSectors.cbegin(), adjacentSectors.cend());
}

bool MapSector::HasAdjacentSector(MapSector* otherSector) const
{
	for (const MapHexTile* tile : _sectorTiles)
	{
		GameObject* tileAdjacentGameObjects[6];
		tile->tile->GetAdjacentGameObjects(tileAdjacentGameObjects);

		for (int i = 0; i < 6; i++)
		{
			if (tileAdjacentGameObjects[i] != nullptr)
			{
				MapHexTile* hexTile = tileAdjacentGameObjects[i]->GetComponent<MapHexTile>();

				if (hexTile != nullptr && hexTile->sector == otherSector)
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
	for (const MapHexTile* tile : _sectorTiles)
	{
		GameObject* tileAdjacentGameObjects[6];
		tile->tile->GetAdjacentGameObjects(tileAdjacentGameObjects);

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

YAML::Node MapSector::Serialize() const
{
	YAML::Node node = Twin2Engine::Core::Component::Serialize();
	node["type"] = "MapSector";
	node.remove("subTypes");
	//node["tilemap"] = tilemap;
	//node["region"] = region;
	node["sectorType"] = type;
	return node;
}