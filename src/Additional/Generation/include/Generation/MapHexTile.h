#ifndef _MAP_HEX_TILE_H_
#define _MAP_HEX_TILE_H_

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>

#include <Tilemap/HexagonalTilemap.h>

namespace Generation
{
	class MapHexTile : public Twin2Engine::Core::Component
	{
		Tilemap::HexagonalTilemap* _tilemap;
		MapRegion* _region;
		MapSector* _sector;

	public:

		inline void SetTilemap(Tilemap::HexagonalTilemap* tilemap);
		inline Tilemap::HexagonalTilemap* GetTilemap() const;

		inline void SetRegion(MapRegion* region);
		inline MapRegion* GetRegion() const;

		inline void SetSector(MapSector* sector);
		inline MapSector* GetSector() const;


	};
}

#endif // !_MAP_HEX_TILE_H_