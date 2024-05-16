#pragma once

#include <Tilemap/HexagonalTilemap.h>

namespace Generation
{
	class MapRegion;
	class MapSector;
	class MapHexTile : public Twin2Engine::Core::Component
	{
		/*CloneFunctionStart(MapHexTile, Twin2Engine::Core::Component)
			//CloneField(tilemap)
			//CloneField(region)
			//CloneField(sector)
			//CloneField(tile)
			CloneField(type)
		CloneFunctionEnd()*/
	protected:
		CloneBaseFunc(MapHexTile, Twin2Engine::Core::Component,
			type
		)
	public:

		enum HexTileType
		{
			None,
			Empty,
			PointOfInterest,
			Water,
			Mountain,
			RadioStation
		};

	public:
		Tilemap::HexagonalTilemap* tilemap = nullptr;
		MapRegion* region = nullptr;
		MapSector* sector = nullptr;
		Tilemap::HexagonalTile* tile = nullptr;
		HexTileType type = HexTileType::None;

		//inline void SetTilemap(Tilemap::HexagonalTilemap* tilemap);
		//inline Tilemap::HexagonalTilemap* GetTilemap() const;
		//
		//inline void SetRegion(MapRegion* region);
		//inline MapRegion* GetRegion() const;
		//
		//inline void SetSector(MapSector* sector);
		//inline MapSector* GetSector() const;


		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

	};
}

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>