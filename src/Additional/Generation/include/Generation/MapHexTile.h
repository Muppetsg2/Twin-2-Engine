#ifndef _MAP_HEX_TILE_H_
#define _MAP_HEX_TILE_H_

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>

#include <Tilemap/HexagonalTilemap.h>

namespace Generation
{
	class MapHexTile : public Twin2Engine::Core::Component
	{

	public:
		Tilemap::HexagonalTilemap* tilemap;
		MapRegion* region;
		MapSector* sector;
		Tilemap::HexagonalTile* tile;

		//inline void SetTilemap(Tilemap::HexagonalTilemap* tilemap);
		//inline Tilemap::HexagonalTilemap* GetTilemap() const;
		//
		//inline void SetRegion(MapRegion* region);
		//inline MapRegion* GetRegion() const;
		//
		//inline void SetSector(MapSector* sector);
		//inline MapSector* GetSector() const;


	};
}

#endif // !_MAP_HEX_TILE_H_