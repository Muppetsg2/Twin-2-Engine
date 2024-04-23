#include <Generation/MapRegion.h>

#include <Generation/YamlConverters.h>


using namespace Generation;
using namespace Tilemap;
using namespace Twin2Engine::Core;
using namespace std;

//void MapRegion::SetTilemap(HexagonalTilemap* tilemap)
//{
//	tilemap = tilemap;
//}
//
//inline HexagonalTilemap* MapRegion::GetTilemap() const
//{
//	return tilemap;
//}
//
//void MapRegion::SetType(const RegionType& type)
//{
//	type = type;
//}
//
//inline MapRegion::RegionType MapRegion::GetType() const
//{
//	return type;
//}


//inline const std::vector<Tilemap::HexagonalTile*>& GetTiles() const;
const std::unordered_set<MapSector*>& MapRegion::GetSectors() const
{
	return _regionSectors;
}
unsigned int MapRegion::GetSectorsCount() const
{
	return static_cast<unsigned int>(_regionSectors.size());
}

void MapRegion::AddSector(MapSector* sector)
{
	_regionSectors.insert(sector);
	sector->region = this;
	sector->GetTransform()->SetParent(GetTransform());
	for (MapHexTile* tile : sector->GetTiles())
	{
		tile->region = this;
	}
}

void MapRegion::AddSectors(const std::vector<MapSector*>& sectors)
{
	_regionSectors.insert(sectors.cbegin(), sectors.cend());
	for (MapSector* sector : sectors)
	{
		sector->region = this;
		sector->GetTransform()->SetParent(GetTransform());
		for (MapHexTile* tile : sector->GetTiles())
		{
			tile->region = this;
		}
	}
}

void MapRegion::RemoveSector(MapSector* sector)
{
	_regionSectors.erase(sector);
}

//void RemoveTiles(const std::vector<Tilemap::HexagonalTile*>& tiles);
void MapRegion::RemoveSectors(const std::unordered_set<MapSector*>& sectors)
{
	for (MapSector* sector : sectors)
	{
		_regionSectors.erase(sector);
	}
}

unsigned int MapRegion::GetTilesCount() const
{
	unsigned int count = 0;
	for (MapSector* sector : _regionSectors)
	{
		count += sector->GetTilesCount();
	}

	return count;
}

void MapRegion::JoinRegion(MapRegion* region)
{
	for (MapSector* sector : region->_regionSectors)
	{
		AddSector(sector);
	}

	region->_regionSectors.clear();
}

std::vector<MapRegion*> MapRegion::GetAdjacentRegions() const
{
	unordered_set<MapRegion*> adjacentRegions;

	for (const MapSector* sector : _regionSectors)
	{
		vector<MapSector*> adjacentSectors = sector->GetAdjacentSectors();

		for (int i = 0; i < adjacentSectors.size(); i++)
		{
			if (adjacentSectors[i]->region != this)
			{
				adjacentRegions.insert(adjacentSectors[i]->region);
			}
		}
	}

	return vector<MapRegion*>(adjacentRegions.cbegin(), adjacentRegions.cend());
}

bool MapRegion::HasAdjacentRegion(MapRegion* otherRegion) const
{
	for (const MapSector* sector : _regionSectors)
	{
		vector<MapSector*> adjacentSectors = sector->GetAdjacentSectors();

		for (int i = 0; i < adjacentSectors.size(); i++)
		{
			if (adjacentSectors[i]->region != otherRegion)
			{
				return true;
			}
		}
	}

	return false;
}

bool MapRegion::IsInternalRegion() const
{
	for (const MapSector* sector : _regionSectors)
	{
		if (!sector->IsInternalSector())
		{
			return false;
		}
	}

	return true;
}

YAML::Node MapRegion::Serialize() const
{
	YAML::Node node = Twin2Engine::Core::Component::Serialize();
	node.remove("type");
	node.remove("subTypes");
	//node["tilemap"] = tilemap;
	node["type"] = type;
	return node;
}