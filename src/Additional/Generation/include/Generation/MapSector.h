#ifndef _MAP_SECTOR_H_
#define _MAP_SECTOR_H_

#include <Tilemap/HexagonalTile.h>
#include <Tilemap/HexagonalTilemap.h>
//#include <Generation/MapRegion.h>
#include <Generation/MapHexTile.h>

using Tilemap::HexagonalTilemap;

namespace Generation
{
    class MapRegion;
    class MapHexTile;

	class MapSector : public Twin2Engine::Core::Component
	{
    public:

        enum SectorType
        {
            Normal,
            Water,
            Mountain
        };

    private:

        //std::vector<Tilemap::HexagonalTile*> _sectorTiles;
        std::unordered_set<MapHexTile*> _sectorTiles;

    public:
        HexagonalTilemap* tilemap = nullptr;
        MapRegion* region = nullptr;
        SectorType type = SectorType::Normal;


        //void SetTilemap(HexagonalTilemap* tilemap);
        //inline HexagonalTilemap* GetTilemap() const;
        //void SetRegion(MapRegion* region);
        //inline MapRegion* GetRegion() const;
        //void SetType(const SectorType& type);
        //inline SectorType GetType();

        //inline const std::vector<MapHexTile*>& GetTiles() const;
        inline const std::unordered_set<MapHexTile*>& GetTiles() const;
        void AddTile(MapHexTile* tile);
        void AddTiles(const std::vector<MapHexTile*>& tiles);
        void JoinSector(MapSector* otherSector);

        void RemoveTile(MapHexTile* tile);
        //void RemoveTiles(const std::vector<MapHexTile*>& tiles);
        void RemoveTiles(const std::unordered_set<MapHexTile*>& tiles);
        size_t GetTilesCount() const;


        std::vector<MapSector*> GetAdjacentSectors() const;
        bool HasAdjacentSector(MapSector* otherSector) const;
        bool IsInternalSector() const;
	};
}

#endif // !_MAP_SECTOR_H_