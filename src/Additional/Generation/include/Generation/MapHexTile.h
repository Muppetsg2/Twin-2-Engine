#pragma once

#include <Tilemap/HexagonalTilemap.h>
#include <GameManager.h>

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

		ENUM_CLASS(HexTileType, None, Empty, PointOfInterest, Water, Mountain, RadioStation)

	public:
		Tilemap::HexagonalTilemap* tilemap = nullptr;
		MapRegion* region = nullptr;
		MapSector* sector = nullptr;
		Tilemap::HexagonalTile* tile = nullptr;
		HexTileType type = HexTileType::None;

		Playable* takenEntity = nullptr;
		float percentage = 0.0f;

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
		virtual void DrawEditor() override;
	};
}

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>