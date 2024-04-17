#ifndef _LAKE_GENERATOR_H_
#define _LAKE_GENERATOR_H_

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>


namespace Generation::Generators
{
    class LakeGenerator : public AMapElementGenerator
    {
    private:
        int numberOfLakes = 0;
        float waterLevel = 0.0f;

        bool destroyWaterTile = true;

    public:
        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
    };
}

#endif // !_LAKE_GENERATOR_H_