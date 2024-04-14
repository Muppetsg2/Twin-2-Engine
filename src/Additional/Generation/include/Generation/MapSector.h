#ifndef _MAP_SECTOR_H_
#define _MAP_SECTOR_H_

#include <Tilemap/HexagonalTile.h>
#include <Tilemap/HexagonalTilemap.h>
#include <Generation/MapRegion.h>
#include <Generation/MapHexTile.h>

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
        HexagonalTilemap* _tilemap;
        MapRegion* _region;

        SectorType _type = SectorType::Normal;

        //std::vector<Tilemap::HexagonalTile*> _sectorTiles;
        std::unordered_set<Tilemap::HexagonalTile*> _sectorTiles;

    public:


        void SetTilemap(HexagonalTilemap* tilemap);
        inline HexagonalTilemap* GetTilemap() const;
        void SetRegion(MapRegion* region);
        inline MapRegion* GetRegion() const;
        void SetType(const SectorType& type);
        inline SectorType GetType();

        //inline const std::vector<Tilemap::HexagonalTile*>& GetTiles() const;
        inline const std::unordered_set<Tilemap::HexagonalTile*>& GetTiles() const;
        void AddTile(Tilemap::HexagonalTile* tile);
        void AddTiles(const std::vector<Tilemap::HexagonalTile*>& tiles);
        void RemoveTile(Tilemap::HexagonalTile* tile);
        //void RemoveTiles(const std::vector<Tilemap::HexagonalTile*>& tiles);
        void RemoveTiles(const std::unordered_set<Tilemap::HexagonalTile*>& tiles);

        std::vector<MapSector*> GetAdjacentSectors() const;
        bool HasAdjacentSector(MapSector* otherSector) const;
        bool IsInternalSector() const;
	};
}

#endif // !_MAP_SECTOR_H_