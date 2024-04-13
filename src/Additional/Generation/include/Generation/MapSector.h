#ifndef _MAP_SECTOR_H_
#define _MAP_SECTOR_H_

#include <Tilemap/HexagonalTilemap.h>
#include <Generation/MapRegion.h>

using Tilemap::HexagonalTilemap;

namespace Generation
{
	class MapSector
	{
    public:

        enum SectorType
        {
            Normal,
            Water,
            Mountain
        };

    private:
        HexagonalTilemap* tilemap;
        MapRegion* region;

        //List<Vector3Int> sectorTilesPositions = new();
        //List<GameObject> sectorTiles = new();
        SectorType type = SectorType::Normal;

    public:

	};
}

#endif // !_MAP_SECTOR_H_