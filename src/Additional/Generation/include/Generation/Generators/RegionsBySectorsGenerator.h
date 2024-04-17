#ifndef _REGIONS_BY_SECTORS_GENERATOR_H_
#define _REGIONS_BY_SECTORS_GENERATOR_H_

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class RegionsBySectorsGenerator : public AMapElementGenerator
    {
    public:
        Twin2Engine::Core::GameObject* regionPrefab;
        bool mergeByNumberTilesPerRegion = false;
        int minTilesPerRegion = 10;
        int maxTilesPerRegion = 20;
        int minSectorsPerRegion = 3;
        int maxSectorsPerRegion = 4;
        bool isDiscritizedHeight = false;
        float lowerHeightRange = 0.0f;
        float upperHeightRange = 0.0f;
        float heightRangeFacor = 1.0f;

        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
    };
}

#endif // !_REGIONS_BY_SECTORS_GENERATOR_H_
