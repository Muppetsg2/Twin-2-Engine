#pragma once

#include <Tilemap/HexagonalTilemap.h>

namespace Generation
{
    class MapSector;

	class MapRegion : public Twin2Engine::Core::Component
	{
        /*CloneFunctionStart(MapRegion, Twin2Engine::Core::Component)
            //CloneField(tilemap)
            CloneField(type)
        CloneFunctionEnd()*/
    protected:
        CloneBaseFunc(MapRegion, Twin2Engine::Core::Component, type)
    public:

        ENUM_CLASS(RegionType, Normal, Water, Mountain)

    private:
        //Tilemap::HexagonalTilemap* _tilemap;
        //
        //RegionType _type;

        //std::vector<MapSector*> _regionSectors;
        std::unordered_set<MapSector*> _regionSectors;

    public:
        Tilemap::HexagonalTilemap* tilemap = nullptr;

        RegionType type = RegionType::Normal;

        //void SetTilemap(Tilemap::HexagonalTilemap* tilemap);
        //inline Tilemap::HexagonalTilemap* GetTilemap() const;
        //void SetType(const RegionType& type);
        //inline RegionType GetType() const;

        //inline const std::vector<Tilemap::HexagonalTile*>& GetTiles() const;
        const std::unordered_set<MapSector*>& GetSectors() const;
        unsigned int GetSectorsCount() const;
        void AddSector(MapSector* tile);
        void AddSectors(const std::vector<MapSector*>& sectors);
        void RemoveSector(MapSector* sector);
        //void RemoveTiles(const std::vector<Tilemap::HexagonalTile*>& tiles);
        void RemoveSectors(const std::unordered_set<MapSector*>& sectors);

        unsigned int GetTilesCount() const;

        void JoinRegion(MapRegion* region);

        std::vector<MapRegion*> GetAdjacentRegions() const;
        bool HasAdjacentRegion(MapRegion* otherRegion) const;
        bool IsInternalRegion() const;


        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
        virtual void DrawEditor() override;
#endif
	};
}

#include <Generation/MapSector.h>
//#include "YamlConverters.h"