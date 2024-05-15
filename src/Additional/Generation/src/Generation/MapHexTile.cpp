#include <Generation/MapHexTile.h>

#include <Generation/YamlConverters.h>

using namespace Generation;
using namespace Tilemap;

using namespace Twin2Engine::Core;

using namespace std;

//inline void MapHexTile::SetTilemap(HexagonalTilemap* tilemap)
//{
//	_tilemap = tilemap;
//}
//
//inline HexagonalTilemap* MapHexTile::GetTilemap() const
//{
//	return _tilemap;
//}
//
//inline void MapHexTile::SetRegion(MapRegion* region)
//{
//	_region = region;
//}
//
//inline MapRegion* MapHexTile::GetRegion() const
//{
//	return _region;
//}
//
//inline void MapHexTile::SetSector(MapSector* sector)
//{
//	_sector = sector;
//}
//
//inline MapSector* MapHexTile::GetSector() const
//{
//	return _sector;
//}

YAML::Node MapHexTile::Serialize() const
{
    YAML::Node node = Twin2Engine::Core::Component::Serialize();
    node["type"] = "MapHexTile";
    //node["tilemap"] = tilemap;
    //node["region"] = region;
    //node["sector"] = sector;
    //node["tile"] = tile;
    node["hexTileType"] = type;
    return node;
}

bool MapHexTile::Deserialize(const YAML::Node& node) {
    if (!node["hexTileType"] || !Component::Deserialize(node)) return false;

    tilemap = nullptr;
    region = nullptr;
    sector = nullptr;
    tile = nullptr;
    type = node["hexTileType"].as<MapHexTile::HexTileType>();

    return true;
}